#include "TextEditorUI.h"

#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace {

class TerminalMode {
public:
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

TextEditorUI::TextEditorUI(const FileManager& fileManager)
    : fileManager_(fileManager) {
}

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
        std::cout << "\nUse the arrow keys to move. Press q to quit.\n";
        positionTerminalCursor();
        std::cout.flush();

        switch (readKey()) {
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
            case Key::Quit:
                running = false;
                break;
            case Key::Unknown:
                break;
        }
    }

    std::cout << "\033[?25h\033[0m\n"; // Show the terminal cursor again.
    std::cout.flush();
}

TextEditorUI::Key TextEditorUI::readKey() const {
    unsigned char firstCharacter;
    if (read(STDIN_FILENO, &firstCharacter, 1) != 1) {
        return Key::Quit;
    }

    if (firstCharacter == 'q') {
        return Key::Quit;
    }

    // Arrow keys arrive as escape sequences such as ESC [ A.
    if (firstCharacter == '\033') {
        unsigned char sequence[2];
        if (read(STDIN_FILENO, sequence, sizeof(sequence)) != sizeof(sequence)) {
            return Key::Unknown;
        }

        if (sequence[0] != '[') {
            return Key::Unknown;
        }

        switch (sequence[1]) {
            case 'A':
                return Key::Up;
            case 'B':
                return Key::Down;
            case 'C':
                return Key::Right;
            case 'D':
                return Key::Left;
            default:
                return Key::Unknown;
        }
    }

    return Key::Unknown;
}

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

void TextEditorUI::positionTerminalCursor() const {
    std::cout << "\033[" << cursorLine_ + 1 << ';' << cursorColumn_ + 1 << 'H';
}

void TextEditorUI::moveUp() {
    if (cursorLine_ > 0) {
        --cursorLine_;
        keepCursorWithinCurrentLine();
    }
}

void TextEditorUI::moveDown() {
    const auto& lines = fileManager_.getLines();
    if (cursorLine_ + 1 < lines.size()) {
        ++cursorLine_;
        keepCursorWithinCurrentLine();
    }
}

void TextEditorUI::moveLeft() {
    if (cursorColumn_ > 0) {
        --cursorColumn_;
    }
}

void TextEditorUI::moveRight() {
    const auto& lines = fileManager_.getLines();
    if (!lines.empty() && cursorColumn_ < lines[cursorLine_].size()) {
        ++cursorColumn_;
    }
}

void TextEditorUI::keepCursorWithinCurrentLine() {
    const auto& lines = fileManager_.getLines();
    if (!lines.empty() && cursorColumn_ > lines[cursorLine_].size()) {
        cursorColumn_ = lines[cursorLine_].size();
    }
}
