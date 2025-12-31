#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>

struct Block {
    size_t start;
    size_t size;
    bool free;
    int id;
};

#endif
