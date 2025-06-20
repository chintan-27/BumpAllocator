# BumpAllocator

A modern, high-performance bump allocator written in C++17/20. Designed for precise control over memory layout, object lifetimes, alignment, and allocation patterns.

## Features
- Typed and aligned allocation
- Destructor tracking
- Multi-buffer fallback
- Thread-safe wrappers
- Custom operator new/delete
- Debug logging and optional UI

## Project Structure
(See directory layout in project plan)

---

## **Directory Layout**

```
BumpAllocator/
├── include/
│   ├── BumpAllocator.hpp         # Core allocator
│   ├── MemoryPool.hpp            # Per-type fixed-size pools
│   ├── MultiBumpAllocator.hpp    # Fallback allocator chain
│   ├── ThreadSafeAllocator.hpp   # Thread-safe wrapper
│   ├── OverloadNew.hpp           # Custom operator new/delete
│   ├── DebugUI.hpp               # Optional curses/ImGui UI
│   └── Logger.hpp                # Lightweight logging
│
├── src/
│   ├── main.cpp                  # Example usage / CLI entry
│   ├── debug_ui.cpp              # Implementation of DebugUI
│   ├── logger.cpp                # Implementation of Logger
│
├── tests/
│   ├── test_alignment.cpp        # Padding/alignment cases
│   ├── test_operator_new.cpp     # Verify new/delete override
│   ├── test_threadsafe.cpp       # Multi-threaded allocations
│   └── test_multibuffer.cpp      # Overflow & fallback tests
│
├── fuzz/
│   └── fuzz_allocator.cpp        # Randomized fuzzer for safety checks
│
├── CMakeLists.txt                # Build system
├── README.md                     # Project overview
└── LICENSE                       # License (MIT recommended)
```

---

### Optional Extras Later

* `examples/` → Multiple CLI or game-style demos
* `docs/` → Diagrams or memory layout explanations
* `tools/` → Scripts to visualize memory or automate builds
