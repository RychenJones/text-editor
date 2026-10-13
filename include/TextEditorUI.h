#ifndef TEXT_EDITOR_UI_H
#define TEXT_EDITOR_UI_H

#include "FileManager.h"

#include <cstddef>

class TextEditorUI {
public:
    // Connects the UI to a file manager.
    explicit TextEditorUI(const FileManager& fileManager);

    // Starts the interactive terminal editor loop.
    void run();

    // Prints the contents and current cursor position to the terminal.
    void displayContents() const;

private:
    enum class Key {
        Up,
        Down,
        Left,
        Right,
        Quit,
        Unknown
    };

    // Reads the next key pressed by the user.
    Key readKey() const;

    // Moves the terminal cursor to the current document position.
    void positionTerminalCursor() const;

    // Moves the cursor up one line when possible.
    void moveUp();

    // Moves the cursor down one line when possible.
    void moveDown();

    // Moves the cursor left one character when possible.
    void moveLeft();

    // Moves the cursor right one character when possible.
    void moveRight();

    // Keeps the cursor column valid after changing lines.
    void keepCursorWithinCurrentLine();

    const FileManager& fileManager_;
    std::size_t cursorLine_ = 0;
    std::size_t cursorColumn_ = 0;
};

#endif // TEXT_EDITOR_UI_H
