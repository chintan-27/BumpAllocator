#pragma once
#include "Logger.hpp"
#include <typeinfo>
#include <cstddef>
#include <new>
#include <vector>
#include <functional>

struct AllocationInfo {
    std::string label;
    std::size_t offset;
    std::size_t size;
};

class BumpAllocator {
public:
    BumpAllocator(char* buffer, std::size_t size);

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t));

    template<typename T>
    T* allocate(const std::string& label = "raw<T>");

    template<typename T, typename... Args>
    T* make(Args&&... args);

    void reset();
    void dumpMemory() const;
    void tag(std::string label, std::size_t start, std::size_t size);

    std::size_t getUsedBytes() const;
    std::size_t getRemainingBytes() const;
    
private:
    std::vector<std::function<void()>> destructors;
    std::vector<AllocationInfo> allocations;

    char* buffer;
    std::size_t capacity;
    std::size_t offset;

};

inline BumpAllocator::BumpAllocator(char* buf, std::size_t size)
    : buffer(buf), capacity(size), offset(0) {}

inline void* BumpAllocator::allocate(std::size_t size, std::size_t alignment) {
    std::uintptr_t current = reinterpret_cast<std::uintptr_t>(buffer + offset);
    std::uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
    std::size_t newOffset = aligned - reinterpret_cast<std::uintptr_t>(buffer) + size;

    if (newOffset > capacity) {
        throw std::bad_alloc();
    }

    offset = newOffset;
    Logger::log("Allocated " + std::to_string(size) + " bytes (alignment " + std::to_string(alignment) + ")");
    return reinterpret_cast<void*>(aligned);
}

template<typename T>
T* BumpAllocator::allocate(const std::string& label) {
    void* mem = allocate(sizeof(T), alignof(T));
    tag(label, reinterpret_cast<char*>(mem) - buffer, sizeof(T));
    return static_cast<T*>(mem);
}

template<typename T, typename... Args>
T* BumpAllocator::make(Args&&... args) {
    T* obj = allocate<T>("make<" + std::string(typeid(T).name()) + ">");
    Logger::log("Constructed object of type '" + std::string{typeid(T).name()} + "'");
    new (obj) T(std::forward<Args>(args)...);
    destructors.emplace_back([obj]() {
        obj->~T();
    });
    return obj;
}

inline void BumpAllocator::reset() {
    for (auto it = destructors.rbegin(); it != destructors.rend(); ++it) {
        (*it)();  // Call destructor
    }
    Logger::log("Resetting allocator — invoking " + std::to_string(destructors.size()) + " destructor(s)");
    destructors.clear();
    offset = 0;
}

inline std::size_t BumpAllocator::getUsedBytes() const {
    return offset;
}

inline std::size_t BumpAllocator::getRemainingBytes() const {
    return capacity - offset;
} 

inline void BumpAllocator::tag(std::string label, std::size_t start, std::size_t size) {
    allocations.push_back({label, start, size});
}

inline void BumpAllocator::dumpMemory() const {
    std::cout << "\nMemory Dump\n";
    for (const auto& alloc : allocations) {
        std::cout << "  [" << alloc.label << "] Offset: " << alloc.offset
                  << ", Size: " << alloc.size << " bytes\n";

        const unsigned char* data = reinterpret_cast<const unsigned char*>(buffer + alloc.offset);
        for (std::size_t i = 0; i < alloc.size; ++i) {
            if (i % 16 == 0) std::cout << "\n  " << std::hex << alloc.offset + i << ": ";
            std::cout << std::hex << static_cast<int>(data[i]) << " ";
        }
        std::cout << "\n\n";
    }
}
