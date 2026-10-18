#include "FileStorage.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

int main() {
    const auto uniqueSuffix = std::chrono::high_resolution_clock::now()
                                  .time_since_epoch()
                                  .count();
    const std::filesystem::path testDirectory =
        std::filesystem::temp_directory_path() /
        ("text_editor_file_storage_tests_" + std::to_string(uniqueSuffix));
    std::filesystem::create_directories(testDirectory);

    const std::filesystem::path existingPath = testDirectory / "existing.txt";
    const std::filesystem::path savedPath = testDirectory / "saved-as.txt";
    const std::filesystem::path newPath = testDirectory / "new.txt";

    {
        std::ofstream existingFile(existingPath);
        existingFile << "first line\nsecond line";
    }

    FileStorage storage;
    Document document;
    assert(storage.openFile(existingPath, document));
    assert((document.getLines() ==
            std::vector<std::string>{"first line", "second line"}));
    assert(!document.hasUnsavedChanges());

    CursorPosition cursor{0, 10};
    assert(document.insertCharacter(cursor, '!'));
    assert(document.hasUnsavedChanges());
    assert(storage.saveFile(savedPath, document));
    assert(!document.hasUnsavedChanges());

    Document savedDocument;
    assert(storage.openFile(savedPath, savedDocument));
    assert((savedDocument.getLines() ==
            std::vector<std::string>{"first line!", "second line"}));

    Document newDocument;
    assert(storage.createFile(newPath, newDocument));
    assert(newDocument.hasUnsavedChanges());
    assert(!std::filesystem::exists(newPath));
    assert(storage.saveFile(newPath, newDocument));
    assert(std::filesystem::exists(newPath));
    assert(!newDocument.hasUnsavedChanges());

    std::error_code error;
    std::filesystem::remove_all(testDirectory, error);
    assert(!error);
    return 0;
}
