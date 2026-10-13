#include "FileManager.h"
#include "TextEditorUI.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    FileManager fileManager;

    std::string filePath;
    if (argc > 1) {
        filePath = argv[1];
    } else {
        std::cout << "Enter the path to a text file: ";
        std::getline(std::cin, filePath);
    }

    if (!fileManager.openFile(filePath)) {
        std::cerr << "Unable to open file: " << filePath << '\n';
        return 1;
    }

    TextEditorUI ui(fileManager);
    ui.run();

    return 0;
}
