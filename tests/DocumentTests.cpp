#include "Document.h"

#include <cassert>
#include <string>
#include <vector>

int main() {
    Document document;
    CursorPosition cursor;

    assert(document.getLines() == std::vector<std::string>{""});
    assert(!document.hasUnsavedChanges());

    assert(document.insertCharacter(cursor, 'h'));
    assert(document.insertCharacter(cursor, 'i'));
    assert(document.getLines() == std::vector<std::string>{"hi"});
    assert(cursor.line == 0 && cursor.column == 2);

    assert(document.insertNewline(cursor));
    assert((document.getLines() == std::vector<std::string>{"hi", ""}));
    assert(cursor.line == 1 && cursor.column == 0);

    assert(document.backspace(cursor));
    assert(document.getLines() == std::vector<std::string>{"hi"});
    assert(cursor.line == 0 && cursor.column == 2);

    document.replaceLines({"abc", "def"});
    cursor = {0, 3};
    assert(document.deleteForward(cursor));
    assert((document.getLines() == std::vector<std::string>{"abcdef"}));

    cursor = {0, 3};
    assert(document.backspace(cursor));
    assert((document.getLines() == std::vector<std::string>{"abdef"}));
    assert(cursor.line == 0 && cursor.column == 2);

    document.startNewDocument();
    assert(document.hasUnsavedChanges());
    document.markSaved();
    assert(!document.hasUnsavedChanges());

    return 0;
}
