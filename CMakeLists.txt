cmake_minimum_required(VERSION 3.10)
project(BumpAllocator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include headers
include_directories(include)

# Build the main demo executable
add_executable(bumpalloc
    src/main.cpp
    src/logger.cpp      # placeholder for Day 2
    src/debug_ui.cpp    # placeholder for Day 5
)
