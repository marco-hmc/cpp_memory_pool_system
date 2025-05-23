#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <utility>

namespace Mp1 {
    struct Block {
        std::atomic<Block*> next;
        alignas(std::max_align_t) char padding[sizeof(std::max_align_t)]{};

        Block() : next(nullptr) {}
        explicit Block(Block* nextBlock) : next(nextBlock) {}
    };

    struct SlotList {
        SlotList* next;  // 修改为 SlotList* 类型
    };

    class MemoryPool {
      public:
        explicit MemoryPool(size_t blockSize = 4096, size_t slotSize = 64);
        ~MemoryPool();

        void* allocate();
        void deallocate(void* ptr);

      private:
        bool freeListIsEmpty();
        void allocateBlock();
        bool pushFreeList(void* slot);
        void* popFreeList();

        void* nextSlot();

      private:
        size_t m_blockSize;
        size_t m_slotSize;
        Block* m_firstBlock;
        std::atomic<SlotList*> m_freeList;

        std::mutex m_mutexForBlock;
    };

    class HashBucket {
      public:
        static MemoryPool& getMemoryPool(int index);

        static void* useMemory(size_t size);

        static void freeMemory(void* ptr, size_t size);

        template <typename T, typename... Args>
        friend T* newElement(Args&&... args);

        template <typename T>
        friend void deleteElement(T* p);
    };

    template <typename T, typename... Args>
    T* newElement(Args&&... args) {
        T* p = reinterpret_cast<T*>(HashBucket::useMemory(sizeof(T)));
        if (p) {
            new (p) T(std::forward<Args>(args)...);
        }
        return p;
    }

    template <typename T>
    void deleteElement(T* p) {
        if (p) {
            p->~T();
            HashBucket::freeMemory(reinterpret_cast<void*>(p), sizeof(T));
        }
    }
}  // namespace Mp1