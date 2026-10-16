#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "CursorPosition.h"

#include <string>
#include <vector>

// In-memory text document and editing operations.
//
// Document intentionally knows nothing about files or terminals. This makes
// its behavior easy to test independently from the user interface.
class Document {
public:
    // Creates a document containing one empty line.
    Document();

    // Replaces the document contents with lines loaded from storage.
    void replaceLines(std::vector<std::string> lines);

    // Starts a new, empty document that has not been saved yet.
    void startNewDocument();

    // Returns the document lines as read-only data.
    const std::vector<std::string>& getLines() const;

    // Returns whether the document changed since it was loaded or saved.
    bool hasUnsavedChanges() const;

    // Marks the current contents as successfully saved.
    void markSaved();

    // Restores the previous document state and cursor position, when one
    // exists.
    bool undo(CursorPosition& cursor);

    // Restores the next document state and cursor position, when one exists.
    bool redo(CursorPosition& cursor);

    // Inserts a character at the cursor and moves the cursor right.
    bool insertCharacter(CursorPosition& cursor, char character);

    // Splits the current line at the cursor and moves to the new line.
    bool insertNewline(CursorPosition& cursor);

    // Deletes behind the cursor, joining lines when needed.
    bool backspace(CursorPosition& cursor);

    // Deletes ahead of the cursor, joining lines when needed.
    bool deleteForward(const CursorPosition& cursor);

private:
    struct HistoryEntry {
        std::vector<std::string> lines;
        CursorPosition cursor;
        bool representsSavedVersion = false;
    };

    // Checks whether the cursor is within the document bounds.
    bool isValidPosition(const CursorPosition& cursor) const;

    // Records the current document state after a successful edit.
    void recordEdit(const CursorPosition& cursor);

    // Keeps the current history entry's cursor synchronized with the UI
    // before an edit begins. Cursor movement itself is not an edit.
    void updateCurrentHistoryCursor(const CursorPosition& cursor);

    // Restores a history entry's contents, cursor, and saved status.
    void restoreHistoryEntry(CursorPosition& cursor);

    // Starts a fresh history timeline for a newly loaded or created document.
    void resetHistory(const CursorPosition& cursor, bool representsSavedVersion);

    std::vector<std::string> lines_;
    bool unsavedChanges_ = false;
    std::vector<HistoryEntry> history_;
    std::size_t historyIndex_ = 0;
};

#endif // DOCUMENT_H
