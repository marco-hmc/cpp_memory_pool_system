#include "MemoryPool.h"

#include <gtest/gtest.h>

#include <vector>

TEST(MemoryPoolTest, MemoryPoolTest1) {
    int* p = Mp1::newElement<int>(5);
    EXPECT_EQ(*p, 5);
    Mp1::deleteElement(p);
}

TEST(MemoryPoolTest, AllocateAndDeallocate) {
    Mp1::MemoryPool pool(4096, 64);

    std::vector<void*> allocated;
    for (int i = 0; i < 10; ++i) {
        void* ptr = pool.allocate();
        ASSERT_NE(ptr, nullptr);
        allocated.push_back(ptr);
    }

    for (void* ptr : allocated) {
        pool.deallocate(ptr);
    }

    for (int i = 0; i < 10; ++i) {
        void* ptr = pool.allocate();
        ASSERT_NE(ptr, nullptr);
    }
}

TEST(HashBucketTest, UseAndFreeMemory) {
    void* smallMemory = Mp1::HashBucket::useMemory(128);
    ASSERT_NE(smallMemory, nullptr);
    Mp1::HashBucket::freeMemory(smallMemory, 128);

    void* largeMemory = Mp1::HashBucket::useMemory(1024);
    ASSERT_NE(largeMemory, nullptr);
    Mp1::HashBucket::freeMemory(largeMemory, 1024);
}

TEST(HashBucketTest, NewAndDeleteElement) {
    int* p = Mp1::newElement<int>(42);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(*p, 42);

    Mp1::deleteElement(p);

    struct TestStruct {
        int a, b;
        TestStruct(int x, int y) : a(x), b(y) {}
    };

    TestStruct* obj = Mp1::newElement<TestStruct>(10, 20);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->a, 10);
    EXPECT_EQ(obj->b, 20);

    Mp1::deleteElement(obj);
}
