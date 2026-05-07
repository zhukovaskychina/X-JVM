#include <gtest/gtest.h>

#include "runtime/Object.h"
#include "runtime/heap/JavaHeap.h"

using Runtime::JavaHeap;

TEST(JavaHeapGc, SweepRemovesOnlyUnmarked) {
    JavaHeap heap;
    Runtime::Object* a = heap.createJavaObject(nullptr);
    Runtime::Object* b = heap.createJavaObject(nullptr);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    ASSERT_EQ(heap.totalTrackedObjects(), 2u);

    heap.clearGcMarksOnAllObjects();
    a->setGcMarked(true);
    heap.sweepUnmarkedObjects();

    EXPECT_EQ(heap.totalTrackedObjects(), 1u);
}

TEST(JavaHeapGc, SweepRemovesAllWhenNoneMarked) {
    JavaHeap heap;
    (void)heap.createJavaObject(nullptr);
    (void)heap.createJavaObject(nullptr);
    heap.clearGcMarksOnAllObjects();
    heap.sweepUnmarkedObjects();
    EXPECT_EQ(heap.totalTrackedObjects(), 0u);
}

TEST(JavaHeapGc, CreateJavaArrayObjectIsTracked) {
    JavaHeap heap;
    Runtime::Object* arr = heap.createJavaArrayObject(nullptr);
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(heap.totalTrackedObjects(), 1u);
}

TEST(JavaHeapGc, ShouldCollectAfterAllocationThreshold) {
    JavaHeap heap;
    heap.setGcAllocationThreshold(2);
    EXPECT_EQ(heap.allocationsSinceLastGc(), 0u);
    (void)heap.createJavaObject(nullptr);
    EXPECT_EQ(heap.allocationsSinceLastGc(), 1u);
    EXPECT_FALSE(heap.shouldCollect());
    (void)heap.createJavaObject(nullptr);
    EXPECT_TRUE(heap.shouldCollect());
    heap.onGcCompleted();
    EXPECT_FALSE(heap.shouldCollect());
    EXPECT_EQ(heap.allocationsSinceLastGc(), 0u);
}
