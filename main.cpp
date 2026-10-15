#include "Document.h"
#include "FileStorage.h"
#include "TextEditorUI.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace {

// Presents the startup choices and prepares the selected document.
bool chooseDocument(FileStorage& fileStorage,
                    Document& document,
                    std::filesystem::path& filePath) {
    while (true) {
        std::cout << "Text Editor\n"
                  << "1. Create a new file\n"
                  << "2. Edit an existing file\n"
                  << "q. Quit\n"
                  << "Choose an option: ";

        std::string choice;
        if (!std::getline(std::cin, choice)) {
            return false;
        }

        if (choice == "q" || choice == "Q") {
            return false;
        }

        if (choice != "1" && choice != "2") {
            std::cout << "Please choose 1, 2, or q.\n\n";
            continue;
        }

        std::cout << "Enter the file path: ";
        std::string pathInput;
        if (!std::getline(std::cin, pathInput)) {
            return false;
        }

        filePath = pathInput;
        if (filePath.empty()) {
            std::cout << "The file path cannot be empty.\n\n";
            continue;
        }

        const bool prepared = choice == "1"
                                  ? fileStorage.createFile(filePath, document)
                                  : fileStorage.openFile(filePath, document);
        if (prepared) {
            return true;
        }

        std::cout << fileStorage.getLastError() << "\n\n";
    }
}

} // namespace

// Shows the startup menu and launches a fresh editor session for each file.
int main() {
    while (true) {
        FileStorage fileStorage;
        Document document;
        std::filesystem::path filePath;

        if (!chooseDocument(fileStorage, document, filePath)) {
            return 0;
        }

        TextEditorUI ui(document, fileStorage, filePath);
        ui.runEditor();

        std::cout << "\nReturning to the main menu.\n\n";
    }
}
