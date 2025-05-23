#pragma once

#include <array>
#include <atomic>
#include <cstddef>

#include "common.h"

namespace Mp2 {
    class ThreadCache {
      public:
        static ThreadCache& getInstance() {
            static thread_local ThreadCache instance;
            return instance;
        }

        void* allocate(size_t size);
        void deallocate(void* ptr, size_t size);

      private:
        ThreadCache();

        void* fetchFromCentralCache(size_t size);
        void returnToCentralCache(void* ptr, size_t size);
        bool shouldReturnToCentralCache(size_t size);
        size_t getBatchNumber(size_t size);

      private:
        std::array<std::atomic<void*>, FREE_LIST_SIZE> m_freeList;
        std::array<std::atomic_flag, FREE_LIST_SIZE> m_centralCache;
    };
}  // namespace Mp2