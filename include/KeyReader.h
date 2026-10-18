#ifndef KEY_READER_H
#define KEY_READER_H

#include <memory>
#include <optional>

enum class Key {
    Up,
    Down,
    Left,
    Right,
    Character,
    Enter,
    Tab,
    Backspace,
    Delete,
    Save,
    SaveAs,
    Open,
    Reload,
    NewDocument,
    Undo,
    Redo,
    Search,
    Escape,
    Quit,
    Unknown
};

struct KeyEvent {
    Key key;
    char character = '\0';
};

// Configures the terminal and converts raw input into editor key events.
class KeyReader {
public:
    // Creates a reader that has not enabled raw terminal input yet.
    KeyReader();

    // Restores the terminal settings if raw input was enabled.
    ~KeyReader();

    KeyReader(const KeyReader&) = delete;
    KeyReader& operator=(const KeyReader&) = delete;

    // Enables raw, immediate terminal input.
    bool enableRawInput();

    // Reads one terminal input sequence and converts it to a key event.
    KeyEvent readKey() const;

    // Reads one character without interpreting special key sequences.
    std::optional<char> readCharacter() const;

private:
    // Reads one byte from standard input.
    std::optional<unsigned char> readByte() const;

    struct TerminalState;
    std::unique_ptr<TerminalState> terminalState_;
};

#endif // KEY_READER_H
