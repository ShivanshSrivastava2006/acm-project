#include "buddy_allocator.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace {

// check if x is power of two
bool is_power_of_two(size_t x) {
    return x > 0 && (x & (x - 1)) == 0;
}

// smallest power of two >= x
size_t next_power_of_two(size_t x) {
    size_t p = 1;
    while (p < x) p <<= 1;
    return p;
}

// compute order such that 2^order = size
int order_from_size(size_t size) {
    int order = 0;
    while ((1ULL << order) < size) order++;
    return order;
}

} // anonymous namespace

BuddyAllocator::BuddyAllocator()
    : total_size(0), max_order(0) {}

bool BuddyAllocator::init(size_t size) {
    if (!is_power_of_two(size)) {
        std::cout << "Buddy allocator requires memory size to be a power of two\n";
        return false;
    }

    total_size = size;
    max_order = order_from_size(size);

    free_lists.clear();
    free_lists.resize(max_order + 1);

    // one big free block initially
    BuddyBlock initial;
    initial.start = 0;
    initial.order = max_order;

    free_lists[max_order].push_back(initial);

    return true;
}

void BuddyAllocator::dump() const {
    std::cout << "Buddy Free Lists:\n";
    for (int k = 0; k <= max_order; ++k) {
        size_t block_size = (1ULL << k);
        std::cout << "Order " << k << " (size " << block_size << "): ";

        if (free_lists[k].empty()) {
            std::cout << "empty";
        } else {
            for (const auto& block : free_lists[k]) {
                std::cout << "[" << block.start << "] ";
            }
        }
        std::cout << "\n";
    }
}

long long BuddyAllocator::allocate(size_t size) {
    if (size == 0 || size > total_size)
        return -1;

    size_t req_size = size;
    size_t rounded = 1;
    while (rounded < req_size) rounded <<= 1;

    int req_order = 0;
    while ((1ULL << req_order) < rounded) req_order++;

    // find smallest available block >= req_order
    int curr_order = req_order;
    while (curr_order <= max_order && free_lists[curr_order].empty()) {
        curr_order++;
    }

    if (curr_order > max_order) {
        return -1; // no space
    }

    // take a block from curr_order
    BuddyBlock block = free_lists[curr_order].front();
    free_lists[curr_order].pop_front();

    // split until we reach required order
    while (curr_order > req_order) {
        curr_order--;

        BuddyBlock buddy;
        buddy.start = block.start + (1ULL << curr_order);
        buddy.order = curr_order;

        // left half stays in block
        block.order = curr_order;

        // right half goes to free list
        free_lists[curr_order].push_back(buddy);
    }

    // block is now of required size
    return static_cast<long long>(block.start);
}

bool BuddyAllocator::deallocate(size_t addr, size_t size) {
    if (addr >= total_size || size == 0)
        return false;

    // round size up to power of two
    size_t rounded = 1;
    while (rounded < size) rounded <<= 1;

    int order = 0;
    while ((1ULL << order) < rounded) order++;

    size_t curr_addr = addr;
    int curr_order = order;

    // try merging upward
    while (curr_order < max_order) {
        size_t buddy_addr = curr_addr ^ (1ULL << curr_order);

        auto& free_list = free_lists[curr_order];
        auto it = free_list.end();

        // search for buddy
        for (auto iter = free_list.begin(); iter != free_list.end(); ++iter) {
            if (iter->start == buddy_addr) {
                it = iter;
                break;
            }
        }

        if (it == free_list.end()) {
            break; // buddy not free → stop merging
        }

        // remove buddy from free list
        free_list.erase(it);

        // merge blocks
        curr_addr = std::min(curr_addr, buddy_addr);
        curr_order++;
    }

    // insert merged block
    BuddyBlock block;
    block.start = curr_addr;
    block.order = curr_order;
    free_lists[curr_order].push_back(block);

    return true;
}
