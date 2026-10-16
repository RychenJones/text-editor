#include "TextEditorUI.h"

#include <iostream>
#include <utility>

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
                         statusMessage_);

        const KeyEvent event = keyReader_.readKey();
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
            case Key::Quit:
                if (!document_.hasUnsavedChanges() || confirmQuitWithoutSaving()) {
                    running = false;
                }
                break;
            case Key::Unknown:
                break;
        }
    }

    renderer_.resetTerminal();
}

// Asks whether unsaved changes should be discarded.
bool TextEditorUI::confirmQuitWithoutSaving() const {
    std::cout << "\nUnsaved changes. Quit without saving? [y/n] ";
    std::cout.flush();

    const auto response = keyReader_.readCharacter();
    std::cout << '\n';
    return response.has_value() && (*response == 'y' || *response == 'Y');
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
