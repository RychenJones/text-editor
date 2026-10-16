#ifndef TEXT_EDITOR_UI_H
#define TEXT_EDITOR_UI_H

#include "CursorPosition.h"
#include "Document.h"
#include "FileStorage.h"
#include "KeyReader.h"
#include "TerminalRenderer.h"
#include "Viewport.h"

#include <filesystem>
#include <string>

// Coordinates terminal input, the document model, storage, and rendering.
class TextEditorUI {
public:
    // Connects the editor UI to its document, storage, and file path.
    TextEditorUI(Document& document,
                 FileStorage& fileStorage,
                 std::filesystem::path filePath);

    // Starts the interactive terminal editor loop.
    void runEditor();

private:
    // Confirms whether the user wants to quit with unsaved changes.
    bool confirmQuitWithoutSaving() const;

    // Moves the cursor one line upward.
    void moveUp();

    // Moves the cursor one line downward.
    void moveDown();

    // Moves the cursor one character left.
    void moveLeft();

    // Moves the cursor one character right.
    void moveRight();

    // Keeps the cursor within the length of its current line.
    void keepCursorWithinCurrentLine();

    // Updates the viewport dimensions if the terminal was resized.
    void updateViewportSize();

    // Scrolls the viewport as needed to keep the cursor visible.
    void keepCursorVisible();

    // Builds the status bar text for the current editor state.
    std::string buildStatusBar() const;

    // Inserts one character through the document model.
    void insertCharacter(char character);

    // Inserts a newline through the document model.
    void insertNewline();

    // Deletes behind the cursor through the document model.
    void backspace();

    // Deletes ahead of the cursor through the document model.
    void deleteForward();

    Document& document_;
    FileStorage& fileStorage_;
    std::filesystem::path filePath_;
    CursorPosition cursor_;
    std::string statusMessage_;
    KeyReader keyReader_;
    TerminalRenderer renderer_;
    Viewport viewport_;
};

#endif // TEXT_EDITOR_UI_H
