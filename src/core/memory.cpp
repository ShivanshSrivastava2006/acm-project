#include "memory.h"
#include "../allocator/allocator.h"
#include <iostream>
#include <iomanip>


Memory::Memory()
    : total_size(0), next_id(1), allocator(nullptr),
      alloc_success(0), alloc_failure(0) {}

void Memory::init(size_t size) {
    total_size = size;
    blocks.clear();
    blocks.push_back({0, size, true, -1});
    next_id = 1;
    alloc_success = 0;
    alloc_failure = 0;
}   

int Memory::allocate(size_t size) {
    if (!allocator) {
        alloc_failure++;
        return -1;
    }

    auto it = allocator->select_block(blocks, size);
    if (it == blocks.end()) {
        alloc_failure++;
        return -1;
    }

    int id = next_id++;

    if (it->size > size) {
        Block remaining = {
            it->start + size,
            it->size - size,
            true,
            -1
        };
        it->size = size;
        blocks.insert(std::next(it), remaining);
    }

    it->free = false;
    it->id = id;

    alloc_success++;
    return id;
}


size_t Memory::get_total_memory() const {
    return total_size;
}

size_t Memory::get_used_memory() const {
    size_t used = 0;
    for (const auto& b : blocks)
        if (!b.free) used += b.size;
    return used;
}

size_t Memory::get_free_memory() const {
    return total_size - get_used_memory();
}

double Memory::get_utilization() const {
    if (total_size == 0) return 0.0;
    return (double)get_used_memory() / total_size * 100.0;
}

double Memory::get_external_fragmentation() const {
    size_t total_free = 0;
    size_t largest_free = 0;

    for (const auto& b : blocks) {
        if (b.free) {
            total_free += b.size;
            largest_free = std::max(largest_free, b.size);
        }
    }

    if (total_free == 0) return 0.0;
    return (1.0 - (double)largest_free / total_free) * 100.0;
}


bool Memory::deallocate(int id) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (!it->free && it->id == id) {
            it->free = true;
            it->id = -1;

            // merge with next
            auto next = std::next(it);
            if (next != blocks.end() && next->free) {
                it->size += next->size;
                blocks.erase(next);
            }

            // merge with previous
            if (it != blocks.begin()) {
                auto prev = std::prev(it);
                if (prev->free) {
                    prev->size += it->size;
                    blocks.erase(it);
                }
            }

            return true;
        }
    }
    return false;
}

void Memory::dump() const {
    for (const auto& block : blocks) {
        std::cout << "[0x"
                  << std::hex << std::setw(4) << std::setfill('0') << block.start
                  << " - 0x"
                  << std::hex << std::setw(4) << (block.start + block.size - 1)
                  << "] ";

        if (block.free) {
            std::cout << "FREE\n";
        } else {
            std::cout << "USED (id=" << block.id << ")\n";
        }
    }
}

void Memory::set_allocator(Allocator* alloc) {
    allocator = alloc;
}
