#include "FileManager.h"

#include <fstream>
#include <iostream>
#include <utility>

// Opens a file and stores its contents and path when successful.
bool FileManager::openFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        return false;
    }

    std::vector<std::string> loadedLines;
    std::string line;

    while (std::getline(inputFile, line)) {
        // Remove the carriage return left behind when reading Windows-style
        // line endings on a platform where '\n' is the delimiter.
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        loadedLines.push_back(line);
    }

    if (inputFile.bad()) {
        return false;
    }

    lines_ = std::move(loadedLines);
    filePath_ = filePath;
    return true;
}

// Prints a temporary message used to verify the class is working.
void FileManager::printTestMessage() const {
    std::cout << "FileManager is working." << std::endl;
}

// Returns read-only access to the loaded file lines.
const std::vector<std::string>& FileManager::getLines() const {
    return lines_;
}

// Returns read-only access to the current file path.
const std::string& FileManager::getFilePath() const {
    return filePath_;
}
