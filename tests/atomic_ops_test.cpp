#include <gtest/gtest.h>

#include "runtime/concurrent/AtomicOps.h"

TEST(AtomicOps, CompareAndSwap32) {
    std::atomic<int32_t> cell{1};
    EXPECT_TRUE(Runtime::Concurrent::compareAndSwap32(&cell, 1, 2));
    EXPECT_EQ(cell.load(), 2);
    EXPECT_FALSE(Runtime::Concurrent::compareAndSwap32(&cell, 1, 3));
    EXPECT_EQ(cell.load(), 2);
}
