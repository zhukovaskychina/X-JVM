#include <gtest/gtest.h>
#include <atomic>
#include <thread>

#include "runtime/threading/SafepointCoordinator.h"

using Runtime::Threading::MutatorRegistration;
using Runtime::Threading::SafepointCoordinator;

class SafepointCoordinatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        SafepointCoordinator::instance().resetForTest();
    }
};

TEST_F(SafepointCoordinatorTest, NoMutatorsWaitReturnsImmediately) {
    auto& sp = SafepointCoordinator::instance();
    sp.requestSafepoint();
    sp.waitAllAtSafepoint();
    sp.resumeWorld();
}

TEST_F(SafepointCoordinatorTest, OneMutatorReachesSafepoint) {
    auto& sp = SafepointCoordinator::instance();
    std::atomic<bool> done{false};
    std::atomic<bool> go{false};
    std::thread t([&]() {
        MutatorRegistration reg;
        while (!go.load()) {
            std::this_thread::yield();
        }
        sp.pollSafepoint();
        done = true;
    });
    while (sp.registeredCountForTest() < 1) {
        std::this_thread::yield();
    }
    sp.requestSafepoint();
    go.store(true);
    sp.waitAllAtSafepoint();
    sp.resumeWorld();
    t.join();
    EXPECT_TRUE(done.load());
}

TEST_F(SafepointCoordinatorTest, TwoMutatorsBothArrive) {
    auto& sp = SafepointCoordinator::instance();
    std::atomic<int> ready{0};
    std::atomic<bool> go{false};
    auto worker = [&]() {
        MutatorRegistration reg;
        ready.fetch_add(1);
        while (!go.load()) {
            std::this_thread::yield();
        }
        sp.pollSafepoint();
    };
    std::thread t1(worker);
    std::thread t2(worker);
    while (ready.load() < 2) {
        std::this_thread::yield();
    }
    sp.requestSafepoint();
    go.store(true);
    sp.waitAllAtSafepoint();
    EXPECT_EQ(sp.registeredCountForTest(), 2);
    EXPECT_EQ(sp.arrivedCountForTest(), 2);
    sp.resumeWorld();
    t1.join();
    t2.join();
}
