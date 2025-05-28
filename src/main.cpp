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

int main() {
    constexpr std::size_t SIZE = 1024;
    alignas(std::max_align_t) char buffer[SIZE];

    BumpAllocator allocator(buffer, SIZE);

    // Test allocate<int>()
    int* a = allocator.allocate<int>();
    *a = 42;
    std::cout << "*a = " << *a << "\n";

    // Test make<Foo>()
    Foo* f = allocator.make<Foo>(99);
    std::cout << "Foo value: " << f->x << "\n";

    std::cout << "Used: " << allocator.getUsedBytes()
              << ", Remaining: " << allocator.getRemainingBytes() << "\n";

    // Reset the allocator
    allocator.reset();
    std::cout << "Allocator reset.\n";

    std::cout << "Used: " << allocator.getUsedBytes()
              << ", Remaining: " << allocator.getRemainingBytes() << "\n";

    return 0;
}
