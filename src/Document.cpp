#include "Document.h"

#include <utility>

// Initializes a document with one empty line.
Document::Document()
    : lines_(1, "") {
    resetHistory(CursorPosition{}, true);
}

// Replaces the current contents with lines loaded from storage.
void Document::replaceLines(std::vector<std::string> lines) {
    if (lines.empty()) {
        lines.emplace_back();
    }

    lines_ = std::move(lines);
    unsavedChanges_ = false;
    resetHistory(CursorPosition{}, true);
}

// Resets the document to one empty, unsaved line.
void Document::startNewDocument() {
    lines_.assign(1, "");
    unsavedChanges_ = true;
    resetHistory(CursorPosition{}, false);
}

// Returns the document lines without copying them.
const std::vector<std::string>& Document::getLines() const {
    return lines_;
}

// Reports whether the document has changed since its last save or load.
bool Document::hasUnsavedChanges() const {
    return unsavedChanges_;
}

// Marks the current document contents as saved.
void Document::markSaved() {
    unsavedChanges_ = false;
    for (HistoryEntry& entry : history_) {
        entry.representsSavedVersion = false;
    }
    history_[historyIndex_].representsSavedVersion = true;
}

// Restores the previous document state and cursor position.
bool Document::undo(CursorPosition& cursor) {
    if (historyIndex_ == 0) {
        return false;
    }

    --historyIndex_;
    restoreHistoryEntry(cursor);
    return true;
}

// Restores the next document state and cursor position.
bool Document::redo(CursorPosition& cursor) {
    if (historyIndex_ + 1 >= history_.size()) {
        return false;
    }

    ++historyIndex_;
    restoreHistoryEntry(cursor);
    return true;
}

// Checks whether the cursor points to a valid character position.
bool Document::isValidPosition(const CursorPosition& cursor) const {
    return cursor.line < lines_.size() &&
           cursor.column <= lines_[cursor.line].size();
}

// Inserts a character at the cursor and advances the cursor.
bool Document::insertCharacter(CursorPosition& cursor, char character) {
    if (!isValidPosition(cursor)) {
        return false;
    }

    updateCurrentHistoryCursor(cursor);
    lines_[cursor.line].insert(cursor.column, 1, character);
    ++cursor.column;
    unsavedChanges_ = true;
    recordEdit(cursor);
    return true;
}

// Splits the current line at the cursor and moves to the new line.
bool Document::insertNewline(CursorPosition& cursor) {
    if (!isValidPosition(cursor)) {
        return false;
    }

    updateCurrentHistoryCursor(cursor);
    std::string remainder = lines_[cursor.line].substr(cursor.column);
    lines_[cursor.line].erase(cursor.column);

    // Insert the text after the cursor as the next document line.
    lines_.insert(lines_.begin() + static_cast<std::ptrdiff_t>(cursor.line + 1),
                  std::move(remainder));

    ++cursor.line;
    cursor.column = 0;
    unsavedChanges_ = true;
    recordEdit(cursor);
    return true;
}

// Deletes behind the cursor or joins the current line to the previous line.
bool Document::backspace(CursorPosition& cursor) {
    if (!isValidPosition(cursor)) {
        return false;
    }

    if (cursor.column > 0) {
        updateCurrentHistoryCursor(cursor);
        lines_[cursor.line].erase(cursor.column - 1, 1);
        --cursor.column;
        unsavedChanges_ = true;
        recordEdit(cursor);
        return true;
    }

    if (cursor.line == 0) {
        return false;
    }

    // At column 0 with a previous line available, join the two lines.
    updateCurrentHistoryCursor(cursor);
    const std::size_t previousLine = cursor.line - 1;
    const std::size_t previousLength = lines_[previousLine].size();
    lines_[previousLine] += lines_[cursor.line];
    lines_.erase(lines_.begin() + static_cast<std::ptrdiff_t>(cursor.line));
    cursor.line = previousLine;
    cursor.column = previousLength;
    unsavedChanges_ = true;
    recordEdit(cursor);
    return true;
}

// Deletes ahead of the cursor or joins the next line to the current line.
bool Document::deleteForward(const CursorPosition& cursor) {
    if (!isValidPosition(cursor)) {
        return false;
    }

    // Delete a character when one exists to the right of the cursor.
    if (cursor.column < lines_[cursor.line].size()) {
        updateCurrentHistoryCursor(cursor);
        lines_[cursor.line].erase(cursor.column, 1);
        unsavedChanges_ = true;
        recordEdit(cursor);
        return true;
    }

    if (cursor.line + 1 >= lines_.size()) {
        return false;
    }

    // At the end of a line, forward delete joins the following line.
    updateCurrentHistoryCursor(cursor);
    lines_[cursor.line] += lines_[cursor.line + 1];
    lines_.erase(lines_.begin() + static_cast<std::ptrdiff_t>(cursor.line + 1));
    unsavedChanges_ = true;
    recordEdit(cursor);
    return true;
}

// Records the current contents and cursor after a successful edit.
void Document::recordEdit(const CursorPosition& cursor) {
    history_.erase(history_.begin() + static_cast<std::ptrdiff_t>(historyIndex_ + 1),
                   history_.end());
    history_.push_back({lines_, cursor, false});
    historyIndex_ = history_.size() - 1;
}

// Stores the cursor position that existed immediately before an edit.
void Document::updateCurrentHistoryCursor(const CursorPosition& cursor) {
    history_[historyIndex_].cursor = cursor;
}

// Restores all state represented by the current history entry.
void Document::restoreHistoryEntry(CursorPosition& cursor) {
    const HistoryEntry& entry = history_[historyIndex_];
    lines_ = entry.lines;
    cursor = entry.cursor;
    unsavedChanges_ = !entry.representsSavedVersion;
}

// Replaces the history with the initial state of a loaded or new document.
void Document::resetHistory(const CursorPosition& cursor,
                            bool representsSavedVersion) {
    history_.clear();
    history_.push_back({lines_, cursor, representsSavedVersion});
    historyIndex_ = 0;
}
