#ifndef TEXT_EDITOR_UI_H
#define TEXT_EDITOR_UI_H

#include "FileManager.h"

#include <cstddef>

class TextEditorUI {
public:
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

    Key readKey() const;
    void positionTerminalCursor() const;
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void keepCursorWithinCurrentLine();

    const FileManager& fileManager_;
    std::size_t cursorLine_ = 0;
    std::size_t cursorColumn_ = 0;
};

#endif // TEXT_EDITOR_UI_H
