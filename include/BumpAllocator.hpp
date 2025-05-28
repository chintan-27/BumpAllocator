#pragma once
#include <cstddef>
#include <new>

class BumpAllocator {
public:
    BumpAllocator(char* buffer, std::size_t size);

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t));

    template<typename T>
    T* allocate();

    template<typename T, typename... Args>
    T* make(Args&&... args);

    void reset();

    std::size_t getUsedBytes() const;
    std::size_t getRemainingBytes() const;

private:
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
    return reinterpret_cast<void*>(aligned);
}

template<typename T>
T* BumpAllocator::allocate() {
    void* mem = allocate(sizeof(T), alignof(T));
    return static_cast<T*>(mem);
}

template<typename T, typename... Args>
T* BumpAllocator::make(Args&&... args) {
    T* mem = allocate<T>();
    return new (mem) T(std::forward<Args>(args)...);
}

inline void BumpAllocator::reset() {
    offset = 0;
}

inline std::size_t BumpAllocator::getUsedBytes() const {
    return offset;
}

inline std::size_t BumpAllocator::getRemainingBytes() const {
    return capacity - offset;
}

        