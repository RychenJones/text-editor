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

    std::cout.rdbuf(originalBuffer);

    const std::string output = renderedOutput.str();
    assert(output.find("ne") != std::string::npos);
    assert(output.find("wo") != std::string::npos);
    assert(output.find("zero") == std::string::npos);
    assert(output.find("three") == std::string::npos);
    assert(output.find("\033[2;2H") != std::string::npos);

    return 0;
}
