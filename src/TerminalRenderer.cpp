#include "TerminalRenderer.h"

#include <iostream>

// Redraws the document, status message, and cursor.
void TerminalRenderer::render(const Document& document,
                              const CursorPosition& cursor,
                              const std::string& statusMessage) const {
    std::cout << "\033[2J\033[H";

    // Draw each stored line in document order.
    for (const std::string& line : document.getLines()) {
        std::cout << line << '\n';
    }

    std::cout << "\nUse arrows to move, type to edit, Ctrl-S to save, and Ctrl-Q to quit.\n";
    if (!statusMessage.empty()) {
        std::cout << statusMessage << '\n';
    }

    positionCursor(cursor);
    std::cout.flush();
}

// Makes the terminal cursor visible.
void TerminalRenderer::showCursor() const {
    std::cout << "\033[?25h";
}

// Restores the visible cursor and normal terminal formatting.
void TerminalRenderer::resetTerminal() const {
    std::cout << "\033[?25h\033[0m\n";
    std::cout.flush();
}

// Moves the terminal cursor to the document position.
void TerminalRenderer::positionCursor(const CursorPosition& cursor) const {
    std::cout << "\033[" << cursor.line + 1 << ';' << cursor.column + 1 << 'H';
}
