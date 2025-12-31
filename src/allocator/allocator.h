#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <list>
#include <cstddef>
#include "../core/block.h"

class Allocator {
public:
    virtual ~Allocator() = default;

    virtual std::list<Block>::iterator
    select_block(std::list<Block>& blocks, size_t size) = 0;
};

#endif
