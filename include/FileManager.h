#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <cstddef>

class FileManager {
public:
    // Creates an empty file manager.
    FileManager() = default;

    // Opens an existing text file and loads its contents into memory.
    bool openFile(const std::string& filePath);

    // Prints a temporary message used to test the file manager.
    void printTestMessage() const;

    // Returns the lines loaded from the current file.
    const std::vector<std::string>& getLines() const;

    // Returns the path of the current file.
    const std::string& getFilePath() const;

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
};

#endif // FILE_MANAGER_H
