#include "../include/BumpAllocator.hpp"
#include <iostream>

struct Foo {
    int x;
    Foo(int val) : x(val) {
        std::cout << "Foo constructed with " << x << "\n";
    }
    ~Foo() {
        std::cout << "Foo destructed with " << x << "\n";
    }
};

int main(int argc, char** argv) {
    constexpr std::size_t SIZE = 1024;
    alignas(std::max_align_t) char buffer[SIZE];
    BumpAllocator allocator(buffer, SIZE);

    // Parse flags first
    bool enableLog = false;
    bool showStatus = false;
    bool showDump = false;

    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--log") Logger::enabled = true;
        else if (flag == "--nolog") Logger::enabled = false;
        else if (flag == "--status") showStatus = true;
        else if (flag == "--dump") showDump = true;
    }

    // Actual usage
    int* a = allocator.allocate<int>("int a");
    *a = 42;

    Foo* f = allocator.make<Foo>(99);

    if (showStatus) {
        std::cout << "[Allocator Status] Used: " << allocator.getUsedBytes()
                  << ", Remaining: " << allocator.getRemainingBytes() << "\n";
    }

    if (showDump) {
        allocator.dumpMemory();
    }

    std::cout << "Done\n";
    allocator.reset();
    return 0;
}

