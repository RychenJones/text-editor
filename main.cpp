#include "FileManager.h"
#include "TextEditorUI.h"

#include <iostream>
#include <string>

namespace {

// Presents the startup choices and prepares the selected document.
bool chooseDocument(FileManager& fileManager) {
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
        std::string filePath;
        if (!std::getline(std::cin, filePath)) {
            return false;
        }

        if (filePath.empty()) {
            std::cout << "The file path cannot be empty.\n\n";
            continue;
        }

        if (choice == "1") {
            if (fileManager.createFile(filePath)) {
                return true;
            }

            std::cout << "Unable to create a new file. It may already exist "
                      << "or the path may be invalid.\n\n";
        } else if (fileManager.openFile(filePath)) {
            return true;
        } else {
            std::cout << "Unable to open existing file: " << filePath << "\n\n";
        }
    }
}

} // namespace

// Presents the startup menu and starts the interactive editor.
int main() {
    while (true) {
        FileManager fileManager;

        if (!chooseDocument(fileManager)) {
            return 0;
        }

        TextEditorUI ui(fileManager);
        ui.run();

        std::cout << "\nReturning to the main menu.\n\n";
    }
}
