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

    // Redraws the visible document text, status bar, status message, and cursor.
    void render(const Document& document,
                const CursorPosition& cursor,
                const Viewport& viewport,
                const std::string& statusBarMessage,
                const std::string& statusMessage) const;

    // Redraws the document with an optional transient message below its first
    // line. The message is display-only and never becomes document content.
    void render(const Document& document,
                const CursorPosition& cursor,
                const Viewport& viewport,
                const std::string& statusBarMessage,
                const std::string& statusMessage,
                const std::string& documentHelpMessage) const;

    // Redraws the visible document text and a single footer message.
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
                        const Viewport& viewport,
                        std::size_t documentHelpLineCount) const;
};

#endif // TERMINAL_RENDERER_H
