#include "TerminalRenderer.h"

#include <algorithm>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {

constexpr std::size_t kFallbackTerminalHeight = 24;
constexpr std::size_t kFallbackTerminalWidth = 80;
constexpr std::size_t kFooterLineCount = 2;

} // namespace

// Returns a viewport sized for the current terminal, leaving room for the
// status bar and transient status message.
Viewport TerminalRenderer::getTerminalViewport() const {
    winsize terminalSize{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalSize) == -1 ||
        terminalSize.ws_row == 0 || terminalSize.ws_col == 0) {
        return {0,
                0,
                kFallbackTerminalHeight - kFooterLineCount,
                kFallbackTerminalWidth};
    }

    const std::size_t terminalHeight = terminalSize.ws_row;
    const std::size_t documentHeight = terminalHeight > kFooterLineCount
                                            ? terminalHeight - kFooterLineCount
                                            : 1;
    return {0, 0, documentHeight, terminalSize.ws_col};
}

// Redraws the visible document text, status bar, status message, and cursor.
void TerminalRenderer::render(const Document& document,
                              const CursorPosition& cursor,
                              const Viewport& viewport,
                              const std::string& statusBarMessage,
                              const std::string& statusMessage) const {
    std::cout << "\033[2J\033[H";

    // Draw only the lines and columns that fall inside the viewport.
    const auto& documentLines = document.getLines();
    for (std::size_t row = 0; row < viewport.height; ++row) {
        const std::size_t lineIndex = viewport.topLine + row;
        if (lineIndex < documentLines.size() &&
            viewport.leftColumn < documentLines[lineIndex].size()) {
            std::cout << documentLines[lineIndex].substr(viewport.leftColumn,
                                                          viewport.width);
        }

        // Clear any characters left over from a previous longer line.
        std::cout << "\033[K";
        if (row + 1 < viewport.height) {
            std::cout << '\n';
        }
    }

    const std::size_t footerStart = viewport.height + 1;
    const auto writeFooterLine = [width = viewport.width](const std::string& text) {
        if (width == 0 || text.size() <= width) {
            std::cout << text;
        } else {
            std::cout << text.substr(0, width);
        }
    };

    std::cout << "\033[" << footerStart << ";1H\033[K";
    writeFooterLine(statusBarMessage);
    std::cout << "\033[" << footerStart + 1 << ";1H\033[K";
    writeFooterLine(statusMessage);

    positionCursor(cursor, viewport);
    std::cout.flush();
}

// Redraws the visible document text and a single footer message.
void TerminalRenderer::render(const Document& document,
                              const CursorPosition& cursor,
                              const Viewport& viewport,
                              const std::string& statusMessage) const {
    render(document, cursor, viewport, statusMessage, "");
}

// Redraws using the current terminal size.
void TerminalRenderer::render(const Document& document,
                              const CursorPosition& cursor,
                              const std::string& statusMessage) const {
    render(document, cursor, getTerminalViewport(), statusMessage, "");
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

// Moves the terminal cursor to the document position within the viewport.
void TerminalRenderer::positionCursor(const CursorPosition& cursor,
                                      const Viewport& viewport) const {
    // Clamping keeps rendering safe if a caller supplies a viewport that does
    // not currently contain the cursor. TextEditorUI normally prevents this.
    const std::size_t visibleRow = cursor.line >= viewport.topLine
                                       ? cursor.line - viewport.topLine
                                       : 0;
    const std::size_t visibleColumn = cursor.column >= viewport.leftColumn
                                          ? cursor.column - viewport.leftColumn
                                          : 0;
    const std::size_t row = viewport.height == 0
                                ? 1
                                : std::min(visibleRow + 1, viewport.height);
    const std::size_t column = viewport.width == 0
                                   ? 1
                                   : std::min(visibleColumn + 1, viewport.width);
    std::cout << "\033[" << row << ';' << column << 'H';
}
