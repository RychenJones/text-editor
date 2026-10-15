#ifndef CURSOR_POSITION_H
#define CURSOR_POSITION_H

#include <cstddef>

// A zero-based position in a document.
struct CursorPosition {
    std::size_t line = 0;
    std::size_t column = 0;
};

#endif // CURSOR_POSITION_H
