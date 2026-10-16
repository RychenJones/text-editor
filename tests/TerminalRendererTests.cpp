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

    return 0;
}
