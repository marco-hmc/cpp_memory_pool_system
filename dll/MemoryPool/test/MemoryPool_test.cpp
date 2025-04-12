#include "MemoryPool.h"

#include <gtest/gtest.h>

TEST(MemoryPoolTest, MemoryPoolTest1) {
    int* p = newElement<int>(5);
    EXPECT_EQ(*p, 5);
    deleteElement(p);
}