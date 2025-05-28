// ThreadSafeAllocator.hpp
#pragma once

#include <mutex>
#include "Logger.hpp"

template <typename Allocator>
class ThreadSafeAllocator {
public:
    ThreadSafeAllocator(Allocator& alloc) : allocator(alloc) {}

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) {
        std::lock_guard<std::mutex> lock(mtx);
        Logger::log("[ThreadSafeAllocator] Allocating " + std::to_string(size) + " bytes");
        return allocator.allocate(size, alignment);
    }

    template <typename T>
    T* allocate(const std::string& label = "raw<T>") {
        std::lock_guard<std::mutex> lock(mtx);
        return allocator.template allocate<T>(label);
    }

    template <typename T, typename... Args>
    T* make(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);
        return allocator.template make<T>(std::forward<Args>(args)...);
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mtx);
        allocator.reset();
    }

    void dumpMemory() const {
        std::lock_guard<std::mutex> lock(mtx);
        allocator.dumpMemory();
    }

    std::size_t getUsedBytes() const {
        std::lock_guard<std::mutex> lock(mtx);
        return allocator.getUsedBytes();
    }

    std::size_t getRemainingBytes() const {
        std::lock_guard<std::mutex> lock(mtx);
        return allocator.getRemainingBytes();
    }

private:
    Allocator& allocator;
    mutable std::mutex mtx;
};
