#pragma once

#include "BumpAllocator.hpp"
#include "Logger.hpp"
#include <vector>
#include <functional>

template <typename T>
class MemoryPool {
public:
    MemoryPool(BumpAllocator& allocator, std::size_t capacity)
        : allocator(allocator), capacity(capacity) {
        Logger::log("[MemoryPool] Created pool for type '" + std::string(typeid(T).name()) + "' with capacity " + std::to_string(capacity));
    }

    template <typename... Args>
    T* make(Args&&... args) {
        if (count >= capacity) {
            Logger::log("[MemoryPool] Out of capacity for type '" + std::string(typeid(T).name()) + "'");
            throw std::bad_alloc();
        }

        T* obj = allocator.make<T>(std::forward<Args>(args)...);
        ++count;
        Logger::log("[MemoryPool] Allocated object of type '" + std::string(typeid(T).name()) + "'");
        return obj;
    }
    void reset() {
        count = 0;
        Logger::log("[MemoryPool] Resetting pool of type '" + std::string(typeid(T).name()) + "'");
    }

    std::size_t size() const { return count; }
    std::size_t max_size() const { return capacity; }

private:
    BumpAllocator& allocator;
    std::size_t capacity;
    std::size_t count = 0;
};
