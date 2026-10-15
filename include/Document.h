#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "CursorPosition.h"

#include <string>
#include <vector>

// In-memory text document and editing operations.
//
// Document intentionally knows nothing about files or terminals. This makes
// its behavior easy to test independently from the user interface.
class Document {
public:
    // Creates a document containing one empty line.
    Document();

    // Replaces the document contents with lines loaded from storage.
    void replaceLines(std::vector<std::string> lines);

    // Starts a new, empty document that has not been saved yet.
    void startNewDocument();

    // Returns the document lines as read-only data.
    const std::vector<std::string>& getLines() const;

    // Returns whether the document changed since it was loaded or saved.
    bool hasUnsavedChanges() const;

    // Marks the current contents as successfully saved.
    void markSaved();

    // Inserts a character at the cursor and moves the cursor right.
    bool insertCharacter(CursorPosition& cursor, char character);

    // Splits the current line at the cursor and moves to the new line.
    bool insertNewline(CursorPosition& cursor);

    // Deletes behind the cursor, joining lines when needed.
    bool backspace(CursorPosition& cursor);

    // Deletes ahead of the cursor, joining lines when needed.
    bool deleteForward(const CursorPosition& cursor);

private:
    // Checks whether the cursor is within the document bounds.
    bool isValidPosition(const CursorPosition& cursor) const;

    std::vector<std::string> lines_;
    bool unsavedChanges_ = false;
};

#endif // DOCUMENT_H
