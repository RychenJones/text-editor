#ifndef TEXT_EDITOR_UI_H
#define TEXT_EDITOR_UI_H

#include "CursorPosition.h"
#include "Document.h"
#include "FileStorage.h"
#include "KeyReader.h"
#include "TerminalRenderer.h"
#include "Viewport.h"

#include <filesystem>
#include <string>
#include <vector>

enum class EditorMode {
    Editing,
    SearchPrompt,
    Confirming
};

// Coordinates terminal input, the document model, storage, and rendering.
class TextEditorUI {
public:
    // Connects the editor UI to its document, storage, and file path.
    TextEditorUI(Document& document,
                 FileStorage& fileStorage,
                 std::filesystem::path filePath);

    // Starts the interactive terminal editor loop.
    void runEditor();

private:
    // Processes an input event while the editor is in normal editing mode.
    void processEditingEvent(const KeyEvent& event, bool& running);

    // Processes input used to build and submit a search query.
    void processSearchPromptEvent(const KeyEvent& event);

    // Processes the response to the unsaved-changes confirmation.
    void processConfirmationEvent(const KeyEvent& event, bool& running);

    // Enters the search prompt mode.
    void beginSearch();

    // Collects all case-insensitive matches for the active search query.
    void collectSearchMatches();

    // Selects the first match at or after the current cursor, with wraparound.
    bool selectInitialSearchMatch();

    // Moves among the active search matches.
    void moveToNextSearchMatch();
    void moveToPreviousSearchMatch();

    // Builds the footer message for the current editor mode.
    std::string buildFooterMessage() const;

    // Builds the one-time controls blurb shown below the welcome line.
    std::string buildControlsBlurb() const;

    // Moves the cursor one line upward.
    void moveUp();

    // Moves the cursor one line downward.
    void moveDown();

    // Moves the cursor one character left.
    void moveLeft();

    // Moves the cursor one character right.
    void moveRight();

    // Keeps the cursor within the length of its current line.
    void keepCursorWithinCurrentLine();

    // Updates the viewport dimensions if the terminal was resized.
    void updateViewportSize();

    // Scrolls the viewport as needed to keep the cursor visible.
    void keepCursorVisible();

    // Builds the status bar text for the current editor state.
    std::string buildStatusBar() const;

    // Inserts one character through the document model.
    void insertCharacter(char character);

    // Inserts a newline through the document model.
    void insertNewline();

    // Inserts a tab character through the document model.
    void insertTab();

    // Deletes behind the cursor through the document model.
    void backspace();

    // Deletes ahead of the cursor through the document model.
    void deleteForward();

    Document& document_;
    FileStorage& fileStorage_;
    std::filesystem::path filePath_;
    CursorPosition cursor_;
    std::string statusMessage_;
    std::string searchQuery_;
    std::string searchResultMessage_;
    std::vector<CursorPosition> searchMatches_;
    std::size_t searchMatchIndex_ = 0;
    bool searchActive_ = false;
    bool showControls_ = true;
    EditorMode mode_ = EditorMode::Editing;
    KeyReader keyReader_;
    TerminalRenderer renderer_;
    Viewport viewport_;
};

#endif // TEXT_EDITOR_UI_H
