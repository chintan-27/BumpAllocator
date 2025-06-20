#include "../include/BumpAllocator.hpp"
#include "../include/Logger.hpp"
#include "../include/MemoryPool.hpp"
#include "../include/ThreadSafeAllocator.hpp"
#include <iostream>
#include <string>
#include <thread>

struct Foo {
    int x;
    Foo(int val) : x(val) {
        Logger::log("Foo constructed with " + std::to_string(x));
    }
    ~Foo() {
        Logger::log("Foo destructed with " + std::to_string(x));
    }
};

struct TrackedObject {
    int value;

    TrackedObject(int v) : value(v) {
        Logger::log("TrackedObject constructed with value " + std::to_string(value));
    }

    ~TrackedObject() {
        Logger::log("TrackedObject destructed with value " + std::to_string(value));
    }

    static BumpAllocator* allocator;

    void* operator new(std::size_t size) {
        Logger::log("[Custom New] Allocating " + std::to_string(size) + " bytes for TrackedObject");
        return allocator->allocate(size);
    }

    void operator delete(void* ptr) {
        Logger::log("[Custom Delete] Ignored (BumpAllocator doesn't support free)");
    }
};

BumpAllocator* TrackedObject::allocator = nullptr;

void thread_test(ThreadSafeAllocator<BumpAllocator>& tsAlloc) {
    for (int i = 0; i < 5; ++i) {
        int* val = tsAlloc.allocate<int>("thread_int");
        *val = i;
        Logger::log("[Thread] Wrote: " + std::to_string(*val));
    }
}

int main(int argc, char* argv[]) {
    constexpr std::size_t SIZE = 2048;
    alignas(std::max_align_t) char buffer[SIZE];
    BumpAllocator allocator(buffer, SIZE);
    TrackedObject::allocator = &allocator;

    ThreadSafeAllocator<BumpAllocator> tsAlloc(allocator);

    // CLI flag processing
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--log") {
            Logger::enabled = true;
        } else if (flag == "--nolog") {
            Logger::enabled = false;
        }
    }

    // Basic allocation
    int* a = allocator.allocate<int>();
    *a = 42;
    Logger::log("Allocated int: " + std::to_string(*a));

    // make<T>
    Foo* f = allocator.make<Foo>(99);
    Logger::log("Foo object at: " + std::to_string(reinterpret_cast<std::uintptr_t>(f)));

    // operator new overload
    TrackedObject* obj = new TrackedObject(123);
    Logger::log("TrackedObject value: " + std::to_string(obj->value));

    // MemoryPool
    MemoryPool<Foo> fooPool(allocator, 5);
    Foo* pooled1 = fooPool.make(111);
    Foo* pooled2 = fooPool.make(222);
    Logger::log("MemoryPool created " + std::to_string(fooPool.size()) + " objects");

    // ThreadSafeAllocator test
    std::thread t1(thread_test, std::ref(tsAlloc));
    std::thread t2(thread_test, std::ref(tsAlloc));
    t1.join();
    t2.join();

    // CLI flags after allocation
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--status") {
            std::cout << "[Allocator Status] Used: " << allocator.getUsedBytes()
                      << ", Remaining: " << allocator.getRemainingBytes() << "\n";
        }
        if (flag == "--dump") {
            allocator.dumpMemory();
        }
    }

    allocator.reset();
    return 0;
}
