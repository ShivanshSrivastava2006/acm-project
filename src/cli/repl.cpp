#include "repl.h"
#include "../allocator/first_fit.h"
#include "../allocator/best_fit.h"
#include "../allocator/worst_fit.h"

#include <iostream>
#include <sstream>
#include <string>

void REPL::run() {
    Memory mem;

    FirstFitAllocator firstFit;
    BestFitAllocator bestFit;
    WorstFitAllocator worstFit;

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

        if (cmd == "exit") {
            break;
        }

        else if (cmd == "init") {
            std::string what;
            size_t size;
            ss >> what >> size;

            if (what == "memory") {
                mem.init(size);
                std::cout << "Memory initialized with size " << size << "\n";
            } else {
                std::cout << "Usage: init memory <size>\n";
            }
        }

        else if (cmd == "set") {
            std::string what, type;
            ss >> what >> type;

            if (what != "allocator") {
                std::cout << "Usage: set allocator <first_fit|best_fit|worst_fit>\n";
                continue;
            }

            if (type == "first_fit") {
                mem.set_allocator(&firstFit);
                std::cout << "Allocator set to First Fit\n";
            } 
            else if (type == "best_fit") {
                mem.set_allocator(&bestFit);
                std::cout << "Allocator set to Best Fit\n";
            } 
            else if (type == "worst_fit") {
                mem.set_allocator(&worstFit);
                std::cout << "Allocator set to Worst Fit\n";
            } 
            else {
                std::cout << "Unknown allocator\n";
            }
        }

        else if (cmd == "malloc") {
            size_t size;
            ss >> size;

            int id = mem.allocate(size);
            if (id == -1) {
                std::cout << "Allocation failed\n";
            } else {
                std::cout << "Allocated block id=" << id << "\n";
            }
        }

        else if (cmd == "free") {
            int id;
            ss >> id;

            if (mem.deallocate(id)) {
                std::cout << "Block " << id << " freed\n";
            } else {
                std::cout << "Invalid block id\n";
            }
        }

        else if (cmd == "dump") {
            mem.dump();
        }
        
        else if (cmd == "stats") {
            std::cout << "Total memory: " << mem.get_total_memory() << "\n";
            std::cout << "Used memory: " << mem.get_used_memory() << "\n";
            std::cout << "Free memory: " << mem.get_free_memory() << "\n";
            std::cout << "Memory utilization: "
                    << mem.get_utilization() << "%\n";
            std::cout << "External fragmentation: "
                    << mem.get_external_fragmentation() << "%\n";
        }


        else {
            std::cout << "Unknown command\n";
        }
    }

    std::cout << "Exiting simulator\n";
}
