#include "repl.h"

#include "../core/memory.h"
#include "../allocator/first_fit.h"
#include "../allocator/best_fit.h"
#include "../allocator/worst_fit.h"
#include "../buddy/buddy_allocator.h"

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

void REPL::run() {

    // ------------------ allocator mode ------------------
    enum class AllocatorMode {
        NORMAL,
        BUDDY
    };



    AllocatorMode mode = AllocatorMode::NORMAL;

    // ------------------ core memory ------------------
    Memory mem;

    FirstFitAllocator firstFit;
    BestFitAllocator bestFit;
    WorstFitAllocator worstFit;

    // ------------------ buddy allocator ------------------
    BuddyAllocator buddy;
    bool buddy_initialized = false;

    // buddy: id -> (addr, size)
    std::unordered_map<int, std::pair<size_t, size_t>> buddy_allocs;
    int buddy_next_id = 1;

    // ------------------ REPL ------------------
    std::string line;

    std::cout << "Memory Management Simulator\n";
    std::cout << "Type 'exit' to quit\n";

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        // ------------------ exit ------------------
        if (cmd == "exit") {
            break;
        }

        // ------------------ init memory ------------------
        else if (cmd == "init") {
            std::string what;
            size_t size;
            ss >> what >> size;

            if (what != "memory") {
                std::cout << "Usage: init memory <size>\n";
                continue;
            }

            mem.init(size);
            buddy_initialized = buddy.init(size);

            buddy_allocs.clear();
            buddy_next_id = 1;
            mode = AllocatorMode::NORMAL;

            std::cout << "Memory initialized with size " << size << "\n";
        }

        // ------------------ set allocator ------------------
        else if (cmd == "set") {
            std::string what, type;
            ss >> what >> type;

            if (what != "allocator") {
                std::cout << "Usage: set allocator <first_fit|best_fit|worst_fit|buddy>\n";
                continue;
            }

            if (type == "first_fit") {
                mem.set_allocator(&firstFit);
                mode = AllocatorMode::NORMAL;
                std::cout << "Allocator set to First Fit\n";
            }
            else if (type == "best_fit") {
                mem.set_allocator(&bestFit);
                mode = AllocatorMode::NORMAL;
                std::cout << "Allocator set to Best Fit\n";
            }
            else if (type == "worst_fit") {
                mem.set_allocator(&worstFit);
                mode = AllocatorMode::NORMAL;
                std::cout << "Allocator set to Worst Fit\n";
            }
            else if (type == "buddy") {
                if (!buddy_initialized) {
                    std::cout << "Buddy allocator requires memory size to be a power of two\n";
                } else {
                    mode = AllocatorMode::BUDDY;
                    std::cout << "Allocator set to Buddy\n";
                }
            }
            else {
                std::cout << "Unknown allocator\n";
            }
        }

        // ------------------ malloc ------------------
        else if (cmd == "malloc") {
            size_t size;
            ss >> size;

            if (mode == AllocatorMode::NORMAL) {
                int id = mem.allocate(size);
                if (id == -1)
                    std::cout << "Allocation failed\n";
                else
                    std::cout << "Allocated block id=" << id << "\n";
            }
            else { // BUDDY
                long long addr = buddy.allocate(size);
                if (addr == -1) {
                    std::cout << "Allocation failed\n";
                } else {
                    int id = buddy_next_id++;
                    buddy_allocs[id] = {static_cast<size_t>(addr), size};
                    std::cout << "Allocated block id=" << id
                              << " at address " << addr << "\n";
                }
            }
        }

        // ------------------ free ------------------
        else if (cmd == "free") {
            int id;
            ss >> id;

            if (mode == AllocatorMode::NORMAL) {
                if (mem.deallocate(id))
                    std::cout << "Block " << id << " freed\n";
                else
                    std::cout << "Invalid block id\n";
            }
            else { // BUDDY
                auto it = buddy_allocs.find(id);
                if (it == buddy_allocs.end()) {
                    std::cout << "Invalid block id\n";
                } else {
                    buddy.deallocate(it->second.first, it->second.second);
                    buddy_allocs.erase(it);
                    std::cout << "Block " << id << " freed\n";
                }
            }
        }

        // ------------------ dump ------------------
        else if (cmd == "dump") {
            if (mode == AllocatorMode::NORMAL)
                mem.dump();
            else
                buddy.dump();
        }

        // ------------------ stats ------------------
        else if (cmd == "stats") {
            if (mode == AllocatorMode::NORMAL) {
                std::cout << "Total memory: " << mem.get_total_memory() << "\n";
                std::cout << "Used memory: " << mem.get_used_memory() << "\n";
                std::cout << "Free memory: " << mem.get_free_memory() << "\n";
                std::cout << "Memory utilization: "
                          << mem.get_utilization() << "%\n";
                std::cout << "External fragmentation: "
                          << mem.get_external_fragmentation() << "%\n";
            } else {
                std::cout << "Stats not implemented for Buddy allocator\n";
            }
        }

        // ------------------ unknown ------------------
        else {
            std::cout << "Unknown command\n";
        }
    }

    std::cout << "Exiting simulator\n";
}
