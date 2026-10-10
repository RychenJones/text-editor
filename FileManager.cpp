#include "FileManager.h"

#include <iostream>

bool FileManager::openFile(const std::string& filePath) {
    // TODO: Open the file, read its contents, and populate lines_.
    (void)filePath;
    return false;
}

void FileManager::printTestMessage() const {
    std::cout << "FileManager is working." << std::endl;
}

const std::vector<std::string>& FileManager::getLines() const {
    return lines_;
}

const std::string& FileManager::getFilePath() const {
    return filePath_;
}
