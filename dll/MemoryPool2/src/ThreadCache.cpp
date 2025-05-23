#include "ThreadCache.h"

#include <atomic>

#include "CentralCache.h"

namespace Mp2 {

    ThreadCache::ThreadCache() {
        for (size_t i = 0; i < FREE_LIST_SIZE; ++i) {
            m_freeList[i].store(nullptr, std::memory_order_relaxed);
            m_centralCache[i].clear();
        }
    }

    void* ThreadCache::allocate(size_t size) {
        size_t index = SizeClass::getIndex(size);
        if (index >= FREE_LIST_SIZE) {
            return nullptr;
        }

        void* ptr = m_freeList[index].load(std::memory_order_acquire);
        if (ptr) {
            m_freeList[index].store(
                static_cast<void*>(static_cast<char*>(ptr) + size),
                std::memory_order_release);
            return ptr;
        }

        return fetchFromCentralCache(size);
    }

    void ThreadCache::deallocate(void* ptr, size_t size) {
        if (!ptr || size <= 0) {
            return;
        }

        size_t index = SizeClass::getIndex(size);
        if (index >= FREE_LIST_SIZE) {
            return;
        }

        if (shouldReturnToCentralCache(size)) {
            returnToCentralCache(ptr, size);
        } else {
            m_freeList[index].store(ptr, std::memory_order_release);
        }
    }
    void* ThreadCache::fetchFromCentralCache(size_t size) {
        size_t index = SizeClass::getIndex(size);
        if (index >= FREE_LIST_SIZE) {
            return nullptr;
        }

        size_t batchNum = getBatchNumber(size);
        void* ptr = CentralCache::getInstance().fetchRange(index, batchNum);
        if (ptr) {
            m_centralCache[index] = ptr;
        }
        return ptr;
    }

    void ThreadCache::returnToCentralCache(void* ptr, size_t size) {
        size_t index = SizeClass::getIndex(size);
        if (index >= FREE_LIST_SIZE) {
            return;
        }

        CentralCache::getInstance().returnRange(ptr, size, size);
    }
    bool ThreadCache::shouldReturnToCentralCache(size_t size) {
        return size > PAGE_SIZE / 2;
    }
    size_t ThreadCache::getBatchNumber(size_t size) {
        return (size + PAGE_SIZE - 1) / PAGE_SIZE;
    }

}  // namespace Mp2