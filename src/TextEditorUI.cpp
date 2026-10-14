#include "TextEditorUI.h"

#include <cctype>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace {

class TerminalMode {
public:
    // Switches the terminal to immediate, no-echo input mode.
    bool enable() {
        if (tcgetattr(STDIN_FILENO, &originalMode_) == -1) {
            return false;
        }

        termios interactiveMode = originalMode_;
        interactiveMode.c_lflag &= ~(ICANON | ECHO);
        interactiveMode.c_iflag &= ~(IXON | ICRNL);
        // Keep normal output processing so '\n' returns to column 1 on each row.
        interactiveMode.c_oflag |= (OPOST | ONLCR);
        interactiveMode.c_cc[VMIN] = 1;
        interactiveMode.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &interactiveMode) == -1) {
            return false;
        }

        enabled_ = true;
        return true;
    }

    // Restores the terminal settings when the editor exits.
    ~TerminalMode() {
        if (enabled_) {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalMode_);
        }
    }

private:
    termios originalMode_{};
    bool enabled_ = false;
};

} // namespace

// Creates a UI that reads document contents from the file manager.
TextEditorUI::TextEditorUI(FileManager& fileManager)
    : fileManager_(fileManager) {
}

// Runs the interactive editor until the user presses Ctrl-Q.
void TextEditorUI::run() {
    TerminalMode terminalMode;
    if (!terminalMode.enable()) {
        std::cerr << "Interactive terminal input is unavailable.\n";
        return;
    }

    std::cout << "\033[?25h"; // Show the terminal cursor while the editor is running.

    bool running = true;

    while (running) {
        // Clear the terminal and redraw the document after every key press.
        std::cout << "\033[2J\033[H";
        displayContents();
        std::cout << "\nUse arrows to move, type to edit, Ctrl-S to save, and Ctrl-Q to quit.\n";
        if (!statusMessage_.empty()) {
            std::cout << statusMessage_ << '\n';
        }
        positionTerminalCursor();
        std::cout.flush();

        const KeyEvent event = readKey();
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
                if (fileManager_.saveFile()) {
                    statusMessage_ = "Changes saved to " + fileManager_.getFilePath();
                } else {
                    statusMessage_ = "Unable to save changes to " + fileManager_.getFilePath();
                }
                break;
            case Key::Quit:
                if (!fileManager_.hasUnsavedChanges() || confirmQuitWithoutSaving()) {
                    running = false;
                }
                break;
            case Key::Unknown:
                break;
        }
    }

    std::cout << "\033[?25h\033[0m\n"; // Show the terminal cursor again.
    std::cout.flush();
}

bool TextEditorUI::confirmQuitWithoutSaving() const {
    std::cout << "\nUnsaved changes. Quit without saving? [y/n] ";
    std::cout.flush();

    unsigned char response;
    if (read(STDIN_FILENO, &response, 1) != 1) {
        std::cout << '\n';
        return false;
    }

    std::cout << static_cast<char>(response) << '\n';
    return response == 'y' || response == 'Y';
}

// Reads an editing key, arrow-key escape sequence, or the quit key.
TextEditorUI::KeyEvent TextEditorUI::readKey() const {
    unsigned char firstCharacter;
    if (read(STDIN_FILENO, &firstCharacter, 1) != 1) {
        return {Key::Quit};
    }

    // Ctrl-Q quits while allowing the user to type the letter q.
    if (firstCharacter == 0x11) {
        return {Key::Quit};
    }

    if (firstCharacter == 0x13) {
        return {Key::Save};
    }

    if (firstCharacter == '\r' || firstCharacter == '\n') {
        return {Key::Enter};
    }

    if (firstCharacter == '\b' || firstCharacter == 127) {
        return {Key::Backspace};
    }

    // Arrow keys arrive as escape sequences such as ESC [ A.
    if (firstCharacter == '\033') {
        unsigned char sequence[2];
        if (read(STDIN_FILENO, sequence, sizeof(sequence)) != sizeof(sequence)) {
            return {Key::Unknown};
        }

        if (sequence[0] != '[') {
            return {Key::Unknown};
        }

        // The Delete key arrives as ESC [ 3 ~.
        if (sequence[1] == '3') {
            unsigned char terminator;
            if (read(STDIN_FILENO, &terminator, 1) == 1 && terminator == '~') {
                return {Key::Delete};
            }
            return {Key::Unknown};
        }

        switch (sequence[1]) {
            case 'A':
                return {Key::Up};
            case 'B':
                return {Key::Down};
            case 'C':
                return {Key::Right};
            case 'D':
                return {Key::Left};
            default:
                return {Key::Unknown};
        }
    }

    if (std::isprint(static_cast<unsigned char>(firstCharacter))) {
        return {Key::Character, static_cast<char>(firstCharacter)};
    }

    return {Key::Unknown};
}

// Prints the current file contents to the terminal.
void TextEditorUI::displayContents() const {
    const auto& lines = fileManager_.getLines();

    if (lines.empty()) {
        std::cout << '\n';
        return;
    }

    for (const std::string& line : lines) {
        std::cout << line << '\n';
    }
}

// Positions the terminal cursor at the current line and column.
void TextEditorUI::positionTerminalCursor() const {
    std::cout << "\033[" << cursorLine_ + 1 << ';' << cursorColumn_ + 1 << 'H';
}

// Moves the cursor up one line when it is not at the top.
void TextEditorUI::moveUp() {
    if (cursorLine_ > 0) {
        --cursorLine_;
        keepCursorWithinCurrentLine();
    }
}

// Moves the cursor down one line when it is not at the bottom.
void TextEditorUI::moveDown() {
    const auto& lines = fileManager_.getLines();
    if (cursorLine_ + 1 < lines.size()) {
        ++cursorLine_;
        keepCursorWithinCurrentLine();
    }
}

// Moves the cursor left one character when possible.
void TextEditorUI::moveLeft() {
    if (cursorColumn_ > 0) {
        --cursorColumn_;
    }
}

// Moves the cursor right one character when possible.
void TextEditorUI::moveRight() {
    const auto& lines = fileManager_.getLines();
    if (!lines.empty() && cursorColumn_ < lines[cursorLine_].size()) {
        ++cursorColumn_;
    }
}

// Clamps the cursor column to the length of the current line.
void TextEditorUI::keepCursorWithinCurrentLine() {
    const auto& lines = fileManager_.getLines();
    if (!lines.empty() && cursorColumn_ > lines[cursorLine_].size()) {
        cursorColumn_ = lines[cursorLine_].size();
    }
}

void TextEditorUI::insertCharacter(char character) {
    if (fileManager_.insertCharacter(cursorLine_, cursorColumn_, character)) {
        ++cursorColumn_;
    }
}

void TextEditorUI::insertNewline() {
    if (fileManager_.insertNewline(cursorLine_, cursorColumn_)) {
        ++cursorLine_;
        cursorColumn_ = 0;
    }
}

void TextEditorUI::backspace() {
    fileManager_.backspace(cursorLine_, cursorColumn_);
}

void TextEditorUI::deleteForward() {
    fileManager_.deleteForward(cursorLine_, cursorColumn_);
}
