#ifndef TERMINAL_RENDERER_H
#define TERMINAL_RENDERER_H

#include "CursorPosition.h"
#include "Document.h"
#include "Viewport.h"

#include <string>

// Draws the document and editor messages in a terminal.
class TerminalRenderer {
public:
    // Returns a viewport sized for the current terminal.
    Viewport getTerminalViewport() const;

    // Redraws the visible document text, status message, and cursor.
    void render(const Document& document,
                const CursorPosition& cursor,
                const Viewport& viewport,
                const std::string& statusMessage) const;

    // Redraws using the current terminal size. This overload preserves the
    // renderer's original API for callers that do not manage a viewport.
    void render(const Document& document,
                const CursorPosition& cursor,
                const std::string& statusMessage) const;

    // Makes the terminal cursor visible.
    void showCursor() const;

    // Restores the visible cursor and normal terminal formatting.
    void resetTerminal() const;

private:
    // Moves the terminal cursor to the document position within the viewport.
    void positionCursor(const CursorPosition& cursor,
                        const Viewport& viewport) const;
};

#endif // TERMINAL_RENDERER_H
