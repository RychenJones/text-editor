#include "TerminalRenderer.h"

#include <algorithm>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include <vector>

namespace {

constexpr std::size_t kFallbackTerminalHeight = 24;
constexpr std::size_t kFallbackTerminalWidth = 80;
constexpr std::size_t kFooterLineCount = 2;

std::vector<std::string> splitLines(const std::string& text) {
    if (text.empty()) {
        return {};
    }

    std::vector<std::string> lines;
    std::size_t lineStart = 0;
    while (lineStart <= text.size()) {
        const std::size_t lineEnd = text.find('\n', lineStart);
        if (lineEnd == std::string::npos) {
            lines.push_back(text.substr(lineStart));
            break;
        }

        lines.push_back(text.substr(lineStart, lineEnd - lineStart));
        lineStart = lineEnd + 1;
    }

    return lines;
}

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
    render(document,
           cursor,
           viewport,
           statusBarMessage,
           statusMessage,
           "");
}

// Redraws the document with an optional transient message below its first
// line. The message is shown only at the top of the document so it does not
// affect normal scrolling or become part of the document contents.
void TerminalRenderer::render(const Document& document,
                              const CursorPosition& cursor,
                              const Viewport& viewport,
                              const std::string& statusBarMessage,
                              const std::string& statusMessage,
                              const std::string& documentHelpMessage,
                              std::optional<std::size_t> footerCursorColumn) const {
    std::cout << "\033[2J\033[H";

    // Draw only the lines and columns that fall inside the viewport.
    const auto& documentLines = document.getLines();
    const std::vector<std::string> helpLines = splitLines(documentHelpMessage);
    const bool showDocumentHelp = viewport.topLine == 0 && !helpLines.empty();
    for (std::size_t row = 0; row < viewport.height; ++row) {
        bool isBlankHelpSpacer = false;
        std::size_t documentRow = row;

        if (showDocumentHelp) {
            if (row > 0 && row <= helpLines.size()) {
                std::cout << helpLines[row - 1].substr(0, viewport.width);
            } else if (row == helpLines.size() + 1) {
                isBlankHelpSpacer = true;
            } else if (row > helpLines.size() + 1) {
                documentRow = row - helpLines.size() - 1;
            }
        }

        const std::size_t lineIndex = viewport.topLine + documentRow;
        const bool isHelpRow = showDocumentHelp && row > 0 &&
                               row <= helpLines.size();
        if (!isHelpRow && !isBlankHelpSpacer &&
            lineIndex < documentLines.size() &&
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

    if (footerCursorColumn.has_value()) {
        positionFooterCursor(*footerCursorColumn, viewport);
    } else {
        positionCursor(cursor, viewport, showDocumentHelp ? helpLines.size() : 0);
    }
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
                                      const Viewport& viewport,
                                      std::size_t documentHelpLineCount) const {
    // Clamping keeps rendering safe if a caller supplies a viewport that does
    // not currently contain the cursor. TextEditorUI normally prevents this.
    const std::size_t visibleRow = cursor.line >= viewport.topLine
                                       ? cursor.line - viewport.topLine
                                       : 0;
    const std::size_t helpOffset = viewport.topLine == 0 &&
                                           documentHelpLineCount > 0 &&
                                           cursor.line > 0
                                       ? documentHelpLineCount + 1
                                       : 0;
    const std::size_t visibleColumn = cursor.column >= viewport.leftColumn
                                          ? cursor.column - viewport.leftColumn
                                          : 0;
    const std::size_t row = viewport.height == 0
                                ? 1
                                : std::min(visibleRow + helpOffset + 1,
                                           viewport.height);
    const std::size_t column = viewport.width == 0
                                   ? 1
                                   : std::min(visibleColumn + 1, viewport.width);
    std::cout << "\033[" << row << ';' << column << 'H';
}

// Moves the terminal cursor to an editable position in the footer.
void TerminalRenderer::positionFooterCursor(std::size_t cursorColumn,
                                             const Viewport& viewport) const {
    const std::size_t column = viewport.width == 0
                                   ? 1
                                   : std::min(cursorColumn + 1, viewport.width);
    std::cout << "\033[" << viewport.height + 2 << ';' << column << 'H';
}
