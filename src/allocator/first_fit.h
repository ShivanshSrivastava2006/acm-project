#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include "allocator.h"

class FirstFitAllocator : public Allocator {
public:
    std::list<Block>::iterator
    select_block(std::list<Block>& blocks, size_t size) override;
};

#endif
