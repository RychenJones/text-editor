#ifndef TERMINAL_RENDERER_H
#define TERMINAL_RENDERER_H

#include "CursorPosition.h"
#include "Document.h"

#include <string>

// Draws the document and editor messages in a terminal.
class TerminalRenderer {
public:
    // Redraws the document, status message, and cursor.
    void render(const Document& document,
                const CursorPosition& cursor,
                const std::string& statusMessage) const;

    // Makes the terminal cursor visible.
    void showCursor() const;

    // Restores the visible cursor and normal terminal formatting.
    void resetTerminal() const;

private:
    // Moves the terminal cursor to the document position.
    void positionCursor(const CursorPosition& cursor) const;
};

#endif // TERMINAL_RENDERER_H
