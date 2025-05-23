#include "MemoryPool.h"

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <mutex>

namespace {
    constexpr int MAX_SLOT_SIZE = 512;
}  // namespace

namespace Mp1 {

    MemoryPool::MemoryPool(size_t blockSize, size_t slotSize)
        : m_blockSize(blockSize),
          m_slotSize(slotSize),
          m_firstBlock(nullptr),
          m_freeList(nullptr) {
        assert(blockSize > 0 && slotSize > 0);
        assert(blockSize % slotSize == 0);
        assert(slotSize >= sizeof(void*));
        assert(blockSize >= slotSize);
        allocateBlock();
    }

    MemoryPool::~MemoryPool() {
        while (m_firstBlock) {
            Block* nextBlock = m_firstBlock->next;
            free(m_firstBlock);
            m_firstBlock = nextBlock;
        }
    }

    void* MemoryPool::allocate() {
        if (freeListIsEmpty()) {
            allocateBlock();
        }
        return popFreeList();
    }

    void MemoryPool::deallocate(void* ptr) {
        if (ptr) {
            pushFreeList(ptr);
        }
    }

    void MemoryPool::allocateBlock() {
        std::lock_guard<std::mutex> lock(m_mutexForBlock);
        Block* newBlock = static_cast<Block*>(malloc(m_blockSize));
        newBlock->next = m_firstBlock;
        m_firstBlock = newBlock;

        char* slot = reinterpret_cast<char*>(newBlock) + sizeof(Block);
        char* blockEnd = reinterpret_cast<char*>(newBlock) + m_blockSize;

        while (slot + m_slotSize <= blockEnd) {
            pushFreeList(slot);
            slot += m_slotSize;
        }
    }

    bool MemoryPool::pushFreeList(void* slot) {
        if (slot) {
            auto* newSlot = static_cast<SlotList*>(slot);
            SlotList* oldHead = nullptr;
            do {
                oldHead = m_freeList.load(std::memory_order_acquire);
                newSlot->next = oldHead;
            } while (!m_freeList.compare_exchange_weak(
                oldHead, newSlot, std::memory_order_release,
                std::memory_order_acquire));
            return true;
        }
        return false;
    }

    void* MemoryPool::popFreeList() {
        SlotList* oldHead = m_freeList.load(std::memory_order_acquire);
        while (oldHead) {
            if (m_freeList.compare_exchange_weak(oldHead, oldHead->next,
                                                 std::memory_order_release,
                                                 std::memory_order_acquire)) {
                return oldHead;
            }
        }
        return nullptr;
    }

    bool MemoryPool::freeListIsEmpty() {
        return m_freeList.load(std::memory_order_acquire) == nullptr;
    }

    /*-----------------------------------------------------------------------------------------------------------------------*/

    MemoryPool& HashBucket::getMemoryPool(int index) {
        static MemoryPool memoryPools[4] = {
            MemoryPool(4096, 64), MemoryPool(4096, 128), MemoryPool(4096, 256),
            MemoryPool(4096, 512)};
        return memoryPools[index];
    }

    void* HashBucket::useMemory(size_t size) {
        if (size <= 0) {
            return nullptr;
        }
        if (size > MAX_SLOT_SIZE) {
            return operator new(size);
        }

        int index = 0;
        if (size <= 64) {
            index = 0;
        } else if (size <= 128) {
            index = 1;
        } else if (size <= 256) {
            index = 2;
        } else {
            index = 3;
        }
        return getMemoryPool(index).allocate();
    }

    void HashBucket::freeMemory(void* ptr, size_t size) {
        if (ptr == nullptr) {
            return;
        }
        if (size > MAX_SLOT_SIZE) {
            operator delete(ptr);
            return;
        }

        int index = 0;
        if (size <= 64) {
            index = 0;
        } else if (size <= 128) {
            index = 1;
        } else if (size <= 256) {
            index = 2;
        } else {
            index = 3;
        }
        getMemoryPool(index).deallocate(ptr);
    }
}  // namespace Mp1