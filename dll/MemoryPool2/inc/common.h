#pragma once

#include <algorithm>
#include <cstddef>

namespace Mp2 {
    constexpr size_t ALIGNMENT = 8;
    constexpr size_t MAX_BYTES = 256 * 1024;
    constexpr size_t FREE_LIST_SIZE = MAX_BYTES / ALIGNMENT;
    constexpr size_t PAGE_SIZE = 4096;

    struct BlockHeader {
        size_t size;
        bool inUse;
        BlockHeader* next;
    };

    class SizeClass {
      public:
        static size_t roundUp(size_t bytes) {
            return (bytes + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
        }

        static size_t getIndex(size_t bytes) {
            bytes = std::max(bytes, ALIGNMENT);
            return (bytes + ALIGNMENT - 1) / ALIGNMENT - 1;
        }
    };

}  // namespace Mp2