# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Output binary
TARGET = memsim

# Source files
SRC = \
src/main.cpp \
src/cli/repl.cpp \
src/core/memory.cpp \
src/allocator/first_fit.cpp \
src/allocator/best_fit.cpp \
src/allocator/worst_fit.cpp \
src/buddy/buddy_allocator.cpp \
src/cache/cache.cpp \
src/cache/cache_system.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
