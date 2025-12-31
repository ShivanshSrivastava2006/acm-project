#ifndef MEMORY_H
#define MEMORY_H

#include <list>
#include <cstddef>
#include "block.h"

class Allocator;   // forward declaration

class Memory {
private:
    size_t total_size;
    std::list<Block> blocks;
    int next_id;
    Allocator* allocator;

public:
    Memory();

    void init(size_t size);
    void set_allocator(Allocator* alloc);

    int allocate(size_t size);
    bool deallocate(int id);

    void dump() const;
};

#endif
