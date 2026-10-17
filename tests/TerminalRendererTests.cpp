#include "Document.h"
#include "TerminalRenderer.h"

#include <cassert>
#include <iostream>
#include <sstream>

int main() {
    Document document;
    document.replaceLines({"zero", "one", "two", "three"});

    const Viewport viewport{1, 1, 2, 2};
    const CursorPosition cursor{2, 2};

    std::ostringstream renderedOutput;
    std::streambuf* originalBuffer = std::cout.rdbuf(renderedOutput.rdbuf());

    TerminalRenderer renderer;
    renderer.render(document, cursor, viewport, "");

    const Viewport statusViewport{1, 1, 2, 80};
    renderer.render(document,
                    cursor,
                    statusViewport,
                    "example.txt   |   Ln 3, Col 3   |   4 lines   |   MODIFIED",
                    "Changes saved to example.txt");

    Document welcomeDocument;
    welcomeDocument.replaceLines({"Welcome to the text editor!", "document line"});
    renderer.render(welcomeDocument,
                    CursorPosition{},
                    Viewport{0, 0, 6, 120},
                    "",
                    "",
                    "Controls: arrows move | Enter creates a new line\n"
                    "Ctrl-Q quits");

    std::cout.rdbuf(originalBuffer);

    const std::string output = renderedOutput.str();
    assert(output.find("ne") != std::string::npos);
    assert(output.find("wo") != std::string::npos);
    assert(output.find("zero") == std::string::npos);
    assert(output.find("three") == std::string::npos);
    assert(output.find("\033[2;2H") != std::string::npos);
    assert(output.find("example.txt   |   Ln 3, Col 3   |   4 lines   |   MODIFIED") !=
           std::string::npos);
    assert(output.find("Changes saved to example.txt") != std::string::npos);
    assert(output.find("Use arrows to move") == std::string::npos);
    const std::size_t welcomePosition =
        output.find("Welcome to the text editor!");
    const std::size_t firstHelpPosition =
        output.find("Controls: arrows move | Enter creates a new line");
    const std::size_t secondHelpPosition = output.find("Ctrl-Q quits");
    const std::size_t documentPosition = output.find("document line");
    assert(welcomePosition < firstHelpPosition);
    assert(firstHelpPosition < secondHelpPosition);
    assert(secondHelpPosition < documentPosition);

    const std::string afterSecondHelp = output.substr(secondHelpPosition);
    assert(afterSecondHelp.find("\033[K\n\033[K\n") != std::string::npos);

    return 0;
}
