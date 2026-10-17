#include "TextEditorUI.h"

#include <cctype>
#include <iostream>
#include <utility>

namespace {

constexpr char kWelcomeLine[] = "Welcome to the text editor!";
constexpr char kControlsBlurb[] =
    "Controls: arrows move | Enter creates a new line | Backspace/Delete remove text\n"
    "Ctrl-S saves | Ctrl-F searches | Ctrl-Z/Y undo/redo | Ctrl-Q quits";

} // namespace

// Connects the UI to the active document, storage, and file path.
TextEditorUI::TextEditorUI(Document& document,
                           FileStorage& fileStorage,
                           std::filesystem::path filePath)
    : document_(document),
      fileStorage_(fileStorage),
      filePath_(std::move(filePath)) {
    updateViewportSize();
}

// Runs the interactive editor until the user chooses to quit.
void TextEditorUI::runEditor() {
    if (!keyReader_.enableRawInput()) {
        std::cerr << "Interactive terminal input is unavailable.\n";
        return;
    }

    renderer_.showCursor();

    bool running = true;
    // Render the current state, then process one input event per iteration.
    while (running) {
        updateViewportSize();
        keepCursorVisible();
        renderer_.render(document_,
                         cursor_,
                         viewport_,
                         buildStatusBar(),
                         buildFooterMessage(),
                         buildControlsBlurb());

        const KeyEvent event = keyReader_.readKey();
        switch (mode_) {
            case EditorMode::Editing:
                processEditingEvent(event, running);
                break;
            case EditorMode::SearchPrompt:
                processSearchPromptEvent(event);
                break;
            case EditorMode::Confirming:
                processConfirmationEvent(event, running);
                break;
        }
    }

    renderer_.resetTerminal();
}

// Processes an input event while the editor is in normal editing mode.
void TextEditorUI::processEditingEvent(const KeyEvent& event, bool& running) {
    showControls_ = false;
    if (event.key != Key::Save) {
        statusMessage_.clear();
    }

    switch (event.key) {
        case Key::Up:
            moveUp();
            break;
        case Key::Down:
            moveDown();
            break;
        case Key::Left:
            moveLeft();
            break;
        case Key::Right:
            moveRight();
            break;
        case Key::Character:
            insertCharacter(event.character);
            break;
        case Key::Enter:
            insertNewline();
            break;
        case Key::Backspace:
            backspace();
            break;
        case Key::Delete:
            deleteForward();
            break;
        case Key::Save:
            if (fileStorage_.saveFile(filePath_, document_)) {
                statusMessage_ = "Changes saved to " + filePath_.string();
            } else {
                statusMessage_ = fileStorage_.getLastError();
            }
            break;
        case Key::Undo:
            document_.undo(cursor_);
            break;
        case Key::Redo:
            document_.redo(cursor_);
            break;
        case Key::Search:
            beginSearch();
            break;
        case Key::Quit:
            if (!document_.hasUnsavedChanges()) {
                running = false;
            } else {
                mode_ = EditorMode::Confirming;
            }
            break;
        case Key::Escape:
        case Key::Unknown:
            break;
    }
}

// Processes input used to build and submit a search query.
void TextEditorUI::processSearchPromptEvent(const KeyEvent& event) {
    if (searchActive_) {
        switch (event.key) {
            case Key::Left:
                moveToPreviousSearchMatch();
                break;
            case Key::Right:
                moveToNextSearchMatch();
                break;
            case Key::Character:
                if (event.character == 'q' || event.character == 'Q') {
                    searchActive_ = false;
                    mode_ = EditorMode::Editing;
                    statusMessage_ = "Find mode closed.";
                }
                break;
            case Key::Escape:
                searchActive_ = false;
                mode_ = EditorMode::Editing;
                statusMessage_ = "Find mode closed.";
                break;
            default:
                break;
        }
        return;
    }

    switch (event.key) {
        case Key::Character:
            searchQuery_.push_back(event.character);
            break;
        case Key::Backspace:
            if (!searchQuery_.empty()) {
                searchQuery_.pop_back();
            }
            break;
        case Key::Enter:
            if (searchQuery_.empty()) {
                searchResultMessage_ = "Enter a search term.";
                return;
            }

            collectSearchMatches();
            searchActive_ = true;
            if (selectInitialSearchMatch()) {
                searchResultMessage_ = std::to_string(searchMatches_.size()) +
                                       " match" +
                                       (searchMatches_.size() == 1 ? "" : "es");
            } else {
                searchResultMessage_ = "No matches";
            }
            break;
        case Key::Escape:
            searchQuery_.clear();
            searchResultMessage_.clear();
            mode_ = EditorMode::Editing;
            break;
        default:
            break;
    }
}

// Processes the response to the unsaved-changes confirmation.
void TextEditorUI::processConfirmationEvent(const KeyEvent& event, bool& running) {
    if (event.key != Key::Character) {
        if (event.key == Key::Escape) {
            mode_ = EditorMode::Editing;
        }
        return;
    }

    if (event.character == 'y' || event.character == 'Y') {
        running = false;
    } else if (event.character == 'n' || event.character == 'N') {
        mode_ = EditorMode::Editing;
    }
}

// Enters the search prompt mode and starts with a fresh search term.
void TextEditorUI::beginSearch() {
    searchQuery_.clear();
    searchResultMessage_.clear();
    searchMatches_.clear();
    searchMatchIndex_ = 0;
    searchActive_ = false;
    mode_ = EditorMode::SearchPrompt;
}

// Converts text to a lowercase form for case-insensitive matching.
std::string toLowercase(const std::string& text) {
    std::string lowercaseText;
    lowercaseText.reserve(text.size());
    for (const unsigned char character : text) {
        lowercaseText.push_back(static_cast<char>(std::tolower(character)));
    }
    return lowercaseText;
}

// Collects every case-insensitive occurrence of the active search query.
void TextEditorUI::collectSearchMatches() {
    searchMatches_.clear();
    const std::string lowercaseQuery = toLowercase(searchQuery_);
    if (lowercaseQuery.empty()) {
        return;
    }

    const auto& documentLines = document_.getLines();
    for (std::size_t line = 0; line < documentLines.size(); ++line) {
        const std::string lowercaseLine = toLowercase(documentLines[line]);
        std::size_t searchStart = 0;
        while (true) {
            const std::size_t match = lowercaseLine.find(lowercaseQuery, searchStart);
            if (match == std::string::npos) {
                break;
            }

            searchMatches_.push_back({line, match});
            searchStart = match + 1;
        }
    }
}

// Selects the first match at or after the current cursor, wrapping around.
bool TextEditorUI::selectInitialSearchMatch() {
    if (searchMatches_.empty()) {
        return false;
    }

    for (std::size_t index = 0; index < searchMatches_.size(); ++index) {
        const CursorPosition& match = searchMatches_[index];
        if (match.line > cursor_.line ||
            (match.line == cursor_.line && match.column >= cursor_.column)) {
            searchMatchIndex_ = index;
            cursor_ = match;
            return true;
        }
    }

    searchMatchIndex_ = 0;
    cursor_ = searchMatches_.front();
    return true;
}

// Moves to the next search match, wrapping at the beginning.
void TextEditorUI::moveToNextSearchMatch() {
    if (searchMatches_.empty()) {
        return;
    }

    searchMatchIndex_ = (searchMatchIndex_ + 1) % searchMatches_.size();
    cursor_ = searchMatches_[searchMatchIndex_];
}

// Moves to the previous search match, wrapping at the end.
void TextEditorUI::moveToPreviousSearchMatch() {
    if (searchMatches_.empty()) {
        return;
    }

    if (searchMatchIndex_ == 0) {
        searchMatchIndex_ = searchMatches_.size() - 1;
    } else {
        --searchMatchIndex_;
    }
    cursor_ = searchMatches_[searchMatchIndex_];
}

// Builds the footer message for the current editor mode.
std::string TextEditorUI::buildFooterMessage() const {
    switch (mode_) {
        case EditorMode::SearchPrompt:
            if (!searchActive_) {
                return "Search: " + searchQuery_ +
                       " | Enter: find | Escape: cancel";
            }
            return "Find: \"" + searchQuery_ + "\" | " + searchResultMessage_ +
                   " | Left: previous | Right: next | q: quit";
        case EditorMode::Confirming:
            return "Unsaved changes. Quit without saving? [y/n]";
        case EditorMode::Editing:
            return statusMessage_;
    }

    return {};
}

// Shows the controls blurb below the welcome line on the first frame only.
std::string TextEditorUI::buildControlsBlurb() const {
    const auto& documentLines = document_.getLines();
    if (!showControls_ || documentLines.empty() ||
        documentLines.front() != kWelcomeLine) {
        return {};
    }

    return kControlsBlurb;
}

// Moves the cursor up when another line exists.
void TextEditorUI::moveUp() {
    if (cursor_.line > 0) {
        --cursor_.line;
        keepCursorWithinCurrentLine();
    }
}

// Moves the cursor down when another line exists.
void TextEditorUI::moveDown() {
    const auto& documentLines = document_.getLines();
    if (cursor_.line + 1 < documentLines.size()) {
        ++cursor_.line;
        keepCursorWithinCurrentLine();
    }
}

// Moves the cursor left when it is not at the start of the line.
void TextEditorUI::moveLeft() {
    if (cursor_.column > 0) {
        --cursor_.column;
    }
}

// Moves the cursor right when another character exists on the line.
void TextEditorUI::moveRight() {
    const auto& documentLines = document_.getLines();
    if (!documentLines.empty() &&
        cursor_.column < documentLines[cursor_.line].size()) {
        ++cursor_.column;
    }
}

// Clamps the cursor column to the current line length.
void TextEditorUI::keepCursorWithinCurrentLine() {
    const auto& documentLines = document_.getLines();
    if (!documentLines.empty() &&
        cursor_.column > documentLines[cursor_.line].size()) {
        cursor_.column = documentLines[cursor_.line].size();
    }
}

// Updates the viewport dimensions while preserving its current scroll
// position. This also makes terminal resizing take effect on the next frame.
void TextEditorUI::updateViewportSize() {
    const Viewport terminalViewport = renderer_.getTerminalViewport();
    viewport_.height = terminalViewport.height;
    viewport_.width = terminalViewport.width;
}

// Scrolls vertically and horizontally so the cursor remains on screen.
void TextEditorUI::keepCursorVisible() {
    if (viewport_.height > 0) {
        if (cursor_.line < viewport_.topLine) {
            viewport_.topLine = cursor_.line;
        } else if (cursor_.line - viewport_.topLine >= viewport_.height) {
            viewport_.topLine = cursor_.line - viewport_.height + 1;
        }
    }

    if (viewport_.width > 0) {
        if (cursor_.column < viewport_.leftColumn) {
            viewport_.leftColumn = cursor_.column;
        } else if (cursor_.column - viewport_.leftColumn >= viewport_.width) {
            viewport_.leftColumn = cursor_.column - viewport_.width + 1;
        }
    }
}

// Builds the status bar text for the current editor state.
std::string TextEditorUI::buildStatusBar() const {
    std::string fileName = filePath_.filename().string();
    if (fileName.empty()) {
        fileName = filePath_.empty() ? "[No Name]" : filePath_.string();
    }

    const std::string saveState = document_.hasUnsavedChanges() ? "MODIFIED" : "SAVED";
    return fileName + "   |   Ln " + std::to_string(cursor_.line + 1) +
           ", Col " + std::to_string(cursor_.column + 1) +
           "   |   " + std::to_string(document_.getLines().size()) +
           " lines   |   " + saveState;
}

// Inserts one character through the document model.
void TextEditorUI::insertCharacter(char character) {
    document_.insertCharacter(cursor_, character);
}

// Inserts a newline through the document model.
void TextEditorUI::insertNewline() {
    document_.insertNewline(cursor_);
}

// Deletes behind the cursor through the document model.
void TextEditorUI::backspace() {
    document_.backspace(cursor_);
}

// Deletes ahead of the cursor through the document model.
void TextEditorUI::deleteForward() {
    document_.deleteForward(cursor_);
}
