#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <cstddef>

// The portion of the document currently visible in the terminal.
struct Viewport {
    std::size_t topLine = 0;
    std::size_t leftColumn = 0;
    std::size_t height = 0;
    std::size_t width = 0;
};

#endif // VIEWPORT_H
