#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <cstddef>

struct CacheLine {
    bool valid;
    size_t tag;

    CacheLine() : valid(false), tag(0) {}
};

#endif
