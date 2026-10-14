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

    // Keep one editable line for an empty file so the cursor always has a
    // valid location when the editor starts.
    if (loadedLines.empty()) {
        loadedLines.emplace_back();
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

bool FileManager::insertCharacter(std::size_t line,
                                  std::size_t column,
                                  char character) {
    if (line >= lines_.size() || column > lines_[line].size()) {
        return false;
    }

    lines_[line].insert(column, 1, character);
    return true;
}

bool FileManager::insertNewline(std::size_t line, std::size_t column) {
    if (line >= lines_.size() || column > lines_[line].size()) {
        return false;
    }

    std::string remainder = lines_[line].substr(column);
    lines_[line].erase(column);
    lines_.insert(lines_.begin() + static_cast<std::ptrdiff_t>(line + 1),
                  std::move(remainder));
    return true;
}

bool FileManager::backspace(std::size_t& line, std::size_t& column) {
    if (line >= lines_.size() || column > lines_[line].size()) {
        return false;
    }

    if (column > 0) {
        lines_[line].erase(column - 1, 1);
        --column;
        return true;
    }

    if (line == 0) {
        return false;
    }

    const std::size_t previousLine = line - 1;
    const std::size_t previousLength = lines_[previousLine].size();
    lines_[previousLine] += lines_[line];
    lines_.erase(lines_.begin() + static_cast<std::ptrdiff_t>(line));
    line = previousLine;
    column = previousLength;
    return true;
}

bool FileManager::deleteForward(std::size_t line, std::size_t column) {
    if (line >= lines_.size() || column > lines_[line].size()) {
        return false;
    }

    if (column < lines_[line].size()) {
        lines_[line].erase(column, 1);
        return true;
    }

    if (line + 1 >= lines_.size()) {
        return false;
    }

    lines_[line] += lines_[line + 1];
    lines_.erase(lines_.begin() + static_cast<std::ptrdiff_t>(line + 1));
    return true;
}
