#include "memory.h"
#include <iostream>
#include <iomanip>

Memory::Memory() : total_size(0), next_id(1) {}

void Memory::init(size_t size) {
    total_size = size;
    blocks.clear();
    blocks.push_back({0, size, true, -1});
    next_id = 1;
}

int Memory::allocate(size_t size) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->free && it->size >= size) {
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
            return id;
        }
    }
    return -1;
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
