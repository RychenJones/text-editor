#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

class FileManager {
public:
    FileManager() = default;

    // Opens an existing text file and loads its contents into memory.
    bool openFile(const std::string& filePath);

    // Temporary test method to confirm the class is working.
    void printTestMessage() const;

    const std::vector<std::string>& getLines() const;
    const std::string& getFilePath() const;

private:
    std::string filePath_;
    std::vector<std::string> lines_;
};

#endif // FILE_MANAGER_H
