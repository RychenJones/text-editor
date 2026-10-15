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
        renderer_.render(document_, cursor_, statusMessage_);

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
