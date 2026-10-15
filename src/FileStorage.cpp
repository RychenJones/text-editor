#include "FileStorage.h"

#include <fstream>
#include <vector>
#include <utility>

// Loads a file and replaces the document contents with its lines.
bool FileStorage::openFile(const std::filesystem::path& filePath,
                           Document& document) {
    lastError_.clear();

    if (filePath.empty()) {
        return setErrorAndFail("The file path cannot be empty.");
    }

    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        return setErrorAndFail("Unable to open " + filePath.string() + ".");
    }

    std::vector<std::string> loadedLines;
    std::string line;

    while (std::getline(inputFile, line)) {
        // Remove a carriage return from Windows-style line endings.
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        loadedLines.push_back(line);
    }

    if (inputFile.bad()) {
        return setErrorAndFail("An error occurred while reading " + filePath.string() + ".");
    }

    document.replaceLines(std::move(loadedLines));
    return true;
}

// Starts a new document after confirming the target file does not exist.
bool FileStorage::createFile(const std::filesystem::path& filePath,
                             Document& document) {
    lastError_.clear();

    if (filePath.empty()) {
        return setErrorAndFail("The file path cannot be empty.");
    }

    std::error_code error;
    if (std::filesystem::exists(filePath, error)) {
        return setErrorAndFail("The file already exists.");
    }

    if (error) {
        return setErrorAndFail("Unable to check the file path: " + error.message() + ".");
    }

    document.startNewDocument();
    return true;
}

// Writes the document contents to the target file.
bool FileStorage::saveFile(const std::filesystem::path& filePath,
                           Document& document) {
    lastError_.clear();

    if (filePath.empty()) {
        return setErrorAndFail("The file path cannot be empty.");
    }

    std::ofstream outputFile(filePath, std::ios::trunc);
    if (!outputFile.is_open()) {
        return setErrorAndFail("Unable to save " + filePath.string() + ".");
    }

    const auto& documentLines = document.getLines();
    for (std::size_t index = 0; index < documentLines.size(); ++index) {
        outputFile << documentLines[index];
        if (index + 1 < documentLines.size()) {
            outputFile << '\n';
        }
    }

    if (!outputFile) {
        return setErrorAndFail("An error occurred while writing " + filePath.string() + ".");
    }

    document.markSaved();
    return true;
}

// Returns the most recent storage error message.
const std::string& FileStorage::getLastError() const {
    return lastError_;
}

// Stores an error message and reports failure to the caller.
bool FileStorage::setErrorAndFail(std::string message) {
    lastError_ = std::move(message);
    return false;
}
