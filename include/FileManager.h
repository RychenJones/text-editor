#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

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

private:
    std::string filePath_;
    std::vector<std::string> lines_;
};

#endif // FILE_MANAGER_H
