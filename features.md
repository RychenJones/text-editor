1.
Scrolling and viewport support
The current renderer prints every line, so large files will run past the terminal. Add a Viewport containing:
struct Viewport {
std::size_t topLine = 0;
std::size_t leftColumn = 0;
std::size_t height = 0;
std::size_t width = 0;
};
Then render only visible text and keep the cursor visible. This enables line numbers, Page Up/Page Down, and horizontal scrolling.
2.
A real status bar
Replace the static help text with something like:
example.txt   |   Ln 12, Col 8   |   42 lines   |   MODIFIED
This would make the editor feel substantially more complete with very little code.
3.
Undo and redo
Add Ctrl-Z and Ctrl-Y. The simplest implementation is to store snapshots of the document before edits. A more scalable design is an EditCommand history.
History entries should include:
◦
document contents
◦
cursor position
◦
whether the state represents the saved version
The existing Document class is the natural home for document history.
4.
Search
Add Ctrl-F, accept a search term, and jump to the next match. Start with plain text search; highlighting and replace can come later.
This will require a small editor mode:
enum class EditorMode {
Editing,
SearchPrompt,
Confirming
};
That is better than adding more direct std::cout prompts like the current quit confirmation.
5.
Better navigation
Add:
◦
Home / End
◦
Ctrl+Home / Ctrl+End
◦
Ctrl+Left / Ctrl+Right
◦
Page Up / Page Down
◦
optional “preferred column” when moving vertically
The current movement functions in TextEditorUI.cpp are simple enough to extend.
6.
Auto-indent and tab handling
When Enter is pressed, copy the leading spaces from the previous line. Also recognize Tab, since KeyReader currently treats it as unknown input.
7.
Save As and reopen
The current file path is fixed when TextEditorUI is created. Add commands for:
◦
Save As
◦
Open another file
◦
Reload from disk
◦
Create a new document without returning to the main menu
These features should reuse FileStorage, with the same unsaved-change confirmation already used by quit.