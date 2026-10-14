#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <cstddef>
#include <string>
#include <vector>

class FileManager {
public:
    // Creates an empty file manager.
    FileManager() = default;

    // Opens an existing text file and loads its contents into memory.
    bool openFile(const std::string& filePath);

    // Starts a new in-memory document that will be created when saved.
    bool createFile(const std::string& filePath);

    // Prints a temporary message used to test the file manager.
    void printTestMessage() const;

    // Returns the lines loaded from the current file.
    const std::vector<std::string>& getLines() const;

    // Returns the path of the current file.
    const std::string& getFilePath() const;

    // Writes the current in-memory contents to the current file.
    bool saveFile();

    // Returns whether the document has changed since its last open or save.
    bool hasUnsavedChanges() const;

    // Inserts one character at the specified cursor position.
    bool insertCharacter(std::size_t line,
                         std::size_t column,
                         char character);

    // Splits a line at the specified cursor position.
    bool insertNewline(std::size_t line, std::size_t column);

    // Deletes the character before the cursor, joining lines when necessary.
    // The cursor position is updated to its new location.
    bool backspace(std::size_t& line, std::size_t& column);

    // Deletes the character at the cursor, joining lines when necessary.
    bool deleteForward(std::size_t line, std::size_t column);

private:
    std::string filePath_;
    std::vector<std::string> lines_;
    bool unsavedChanges_ = false;
};

#endif // FILE_MANAGER_H
