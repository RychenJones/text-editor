#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "Document.h"

#include <filesystem>
#include <string>

// Loads and saves Documents. It owns no document data itself.
class FileStorage {
public:
    // Loads an existing file into the document.
    bool openFile(const std::filesystem::path& filePath, Document& document);

    // Prepares a new document for a file that does not yet exist.
    bool createFile(const std::filesystem::path& filePath, Document& document);

    // Writes the document contents to a file.
    bool saveFile(const std::filesystem::path& filePath, Document& document);

    // Describes the reason the most recent operation failed.
    const std::string& getLastError() const;

private:
    // Stores an error message and reports that an operation failed.
    bool setErrorAndFail(std::string message);

    std::string lastError_;
};

#endif // FILE_STORAGE_H
