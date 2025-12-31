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
    size_t alloc_success;
    size_t alloc_failure;


public:
    Memory();

    void init(size_t size);
    void set_allocator(Allocator* alloc);

    int allocate(size_t size);
    bool deallocate(int id);

    void dump() const;
    size_t get_total_memory() const;
    size_t get_used_memory() const;
    size_t get_free_memory() const;
    double get_external_fragmentation() const;
    double get_utilization() const;

};

#endif
