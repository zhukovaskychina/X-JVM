#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "runtime/JavaThread.h"
#include "runtime/Object.h"
#include "runtime/threading/ObjectMonitor.h"

using Runtime::Threading::ObjectMonitorTable;

class ObjectMonitorWaitNotifyTest : public ::testing::Test {
protected:
    void SetUp() override {
        ObjectMonitorTable::instance().resetForTest();
    }
};

TEST_F(ObjectMonitorWaitNotifyTest, HoldsLockReflectsOwnership) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* other = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();

    auto& table = ObjectMonitorTable::instance();
    table.monitorEnter(obj, owner);
    EXPECT_TRUE(table.holdsLock(obj, owner));
    EXPECT_FALSE(table.holdsLock(obj, other));
    table.monitorExit(obj, owner);
    EXPECT_FALSE(table.holdsLock(obj, owner));
}

TEST_F(ObjectMonitorWaitNotifyTest, WaitReleasesAndReacquiresMonitor) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* notifier = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::atomic<bool> woke{false};
    std::thread waiter([&]() {
        table.monitorWait(obj, owner, 0);
        woke.store(true);
        table.monitorExit(obj, owner);
    });

    // wait until ownership released by wait()
    while (table.holdsLock(obj, owner)) {
        std::this_thread::yield();
    }

    table.monitorEnter(obj, notifier);
    table.monitorNotify(obj, notifier);
    table.monitorExit(obj, notifier);

    waiter.join();
    EXPECT_TRUE(woke.load());
}

TEST_F(ObjectMonitorWaitNotifyTest, NotifyWithoutOwnershipThrows) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* other = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    EXPECT_THROW(table.monitorNotify(obj, other), std::runtime_error);
    table.monitorExit(obj, owner);
}

TEST_F(ObjectMonitorWaitNotifyTest, MonitorEntryRemovedAfterFullRelease) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    EXPECT_EQ(table.monitorCountForTest(), 1);
    table.monitorExit(obj, owner);
    EXPECT_EQ(table.monitorCountForTest(), 0);
}

TEST_F(ObjectMonitorWaitNotifyTest, NotifyAllWakesAllWaiters) {
    Runtime::JavaThread* w1 = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* w2 = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* notifier = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    std::atomic<int> woke{0};
    std::atomic<int> entered{0};

    std::thread t1([&]() {
        table.monitorEnter(obj, w1);
        entered.fetch_add(1);
        table.monitorWait(obj, w1, 0);
        woke.fetch_add(1);
        table.monitorExit(obj, w1);
    });
    std::thread t2([&]() {
        table.monitorEnter(obj, w2);
        entered.fetch_add(1);
        table.monitorWait(obj, w2, 0);
        woke.fetch_add(1);
        table.monitorExit(obj, w2);
    });

    while (entered.load() < 2) {
        std::this_thread::yield();
    }
    while (table.holdsLock(obj, w1) || table.holdsLock(obj, w2)) {
        std::this_thread::yield();
    }

    table.monitorEnter(obj, notifier);
    table.monitorNotifyAll(obj, notifier);
    table.monitorExit(obj, notifier);

    t1.join();
    t2.join();
    EXPECT_EQ(woke.load(), 2);
}

TEST_F(ObjectMonitorWaitNotifyTest, NotifyWakesOnlyOneUntilSecondNotify) {
    Runtime::JavaThread* w1 = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* w2 = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* notifier = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    std::atomic<int> woke{0};
    std::atomic<int> entered{0};
    std::thread t1([&]() {
        table.monitorEnter(obj, w1);
        entered.fetch_add(1);
        table.monitorWait(obj, w1, 0);
        woke.fetch_add(1);
        table.monitorExit(obj, w1);
    });
    std::thread t2([&]() {
        table.monitorEnter(obj, w2);
        entered.fetch_add(1);
        table.monitorWait(obj, w2, 0);
        woke.fetch_add(1);
        table.monitorExit(obj, w2);
    });

    while (entered.load() < 2) {
        std::this_thread::yield();
    }
    while (table.holdsLock(obj, w1) || table.holdsLock(obj, w2)) {
        std::this_thread::yield();
    }

    table.monitorEnter(obj, notifier);
    table.monitorNotify(obj, notifier);
    table.monitorExit(obj, notifier);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(woke.load(), 1);

    table.monitorEnter(obj, notifier);
    table.monitorNotify(obj, notifier);
    table.monitorExit(obj, notifier);

    t1.join();
    t2.join();
    EXPECT_EQ(woke.load(), 2);
}

TEST_F(ObjectMonitorWaitNotifyTest, WaitWithTimeoutReturnsWithoutNotify) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    const auto start = std::chrono::steady_clock::now();
    table.monitorWait(obj, owner, 20);
    const auto end = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_TRUE(table.holdsLock(obj, owner));
    EXPECT_GE(ms, 10);
    table.monitorExit(obj, owner);
}

TEST_F(ObjectMonitorWaitNotifyTest, NegativeTimeoutThrows) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    EXPECT_THROW(table.monitorWait(obj, owner, -1), std::runtime_error);
    table.monitorExit(obj, owner);
}

TEST_F(ObjectMonitorWaitNotifyTest, WaitThrowsWhenThreadInterrupted) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    owner->interrupt();
    table.monitorEnter(obj, owner);
    EXPECT_THROW(table.monitorWait(obj, owner, 0), std::runtime_error);
    EXPECT_FALSE(owner->isInterrupted(false));
    table.monitorExit(obj, owner);
}

TEST_F(ObjectMonitorWaitNotifyTest, WaitInterruptedWhileBlockedThrows) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* notifier = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::atomic<bool> threw{false};
    std::thread waiter([&]() {
        try {
            table.monitorWait(obj, owner, 0);
        } catch (const std::runtime_error&) {
            threw.store(true);
        }
        if (table.holdsLock(obj, owner)) {
            table.monitorExit(obj, owner);
        }
    });

    while (table.holdsLock(obj, owner)) {
        std::this_thread::yield();
    }

    owner->interrupt();
    table.monitorEnter(obj, notifier);
    table.monitorNotifyAll(obj, notifier);
    table.monitorExit(obj, notifier);

    waiter.join();
    EXPECT_TRUE(threw.load());
    EXPECT_FALSE(owner->isInterrupted(false));
}

TEST_F(ObjectMonitorWaitNotifyTest, TimedWaitInterruptedWithoutNotifyThrowsEarly) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::atomic<bool> threw{false};
    std::atomic<long long> elapsedMs{0};
    std::thread waiter([&]() {
        const auto start = std::chrono::steady_clock::now();
        try {
            table.monitorWait(obj, owner, 500);
        } catch (const std::runtime_error&) {
            threw.store(true);
        }
        const auto end = std::chrono::steady_clock::now();
        elapsedMs.store(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        if (table.holdsLock(obj, owner)) {
            table.monitorExit(obj, owner);
        }
    });

    while (table.holdsLock(obj, owner)) {
        std::this_thread::yield();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    owner->interrupt();

    waiter.join();
    EXPECT_TRUE(threw.load());
    EXPECT_LT(elapsedMs.load(), 300);
    EXPECT_FALSE(owner->isInterrupted(false));
}

TEST_F(ObjectMonitorWaitNotifyTest, WaitUpdatesThreadStateToWaitingAndBackToRunnable) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* notifier = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::thread waiter([&]() {
        table.monitorWait(obj, owner, 0);
        table.monitorExit(obj, owner);
    });

    while (owner->getThreadState() != Runtime::JavaThreadState::Waiting) {
        std::this_thread::yield();
    }

    table.monitorEnter(obj, notifier);
    table.monitorNotify(obj, notifier);
    table.monitorExit(obj, notifier);
    waiter.join();
    EXPECT_EQ(owner->getThreadState(), Runtime::JavaThreadState::Runnable);
}

TEST_F(ObjectMonitorWaitNotifyTest, TimedWaitUpdatesThreadStateToTimedWaiting) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::thread waiter([&]() {
        table.monitorWait(obj, owner, 50);
        table.monitorExit(obj, owner);
    });

    while (owner->getThreadState() != Runtime::JavaThreadState::TimedWaiting) {
        std::this_thread::yield();
    }
    waiter.join();
    EXPECT_EQ(owner->getThreadState(), Runtime::JavaThreadState::Runnable);
}

TEST_F(ObjectMonitorWaitNotifyTest, ContendedEnterUpdatesThreadStateToBlocked) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* contender = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::thread blocked([&]() {
        table.monitorEnter(obj, contender);
        table.monitorExit(obj, contender);
    });

    while (contender->getThreadState() != Runtime::JavaThreadState::Blocked) {
        std::this_thread::yield();
    }
    table.monitorExit(obj, owner);
    blocked.join();
    EXPECT_EQ(contender->getThreadState(), Runtime::JavaThreadState::Runnable);
}

TEST_F(ObjectMonitorWaitNotifyTest, WaitReenterContentionTransitionsToBlocked) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* notifier = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    table.monitorEnter(obj, owner);
    std::thread waiter([&]() {
        table.monitorWait(obj, owner, 0);
        table.monitorExit(obj, owner);
    });

    while (owner->getThreadState() != Runtime::JavaThreadState::Waiting) {
        std::this_thread::yield();
    }

    std::atomic<bool> notifierHolding{false};
    std::thread notifierThread([&]() {
        table.monitorEnter(obj, notifier);
        notifierHolding.store(true);
        table.monitorNotify(obj, notifier);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        table.monitorExit(obj, notifier);
    });

    while (!notifierHolding.load()) {
        std::this_thread::yield();
    }
    while (owner->getThreadState() != Runtime::JavaThreadState::Blocked) {
        std::this_thread::yield();
    }

    notifierThread.join();
    waiter.join();
    EXPECT_EQ(owner->getThreadState(), Runtime::JavaThreadState::Runnable);
}

TEST_F(ObjectMonitorWaitNotifyTest, IllegalMonitorWaitDoesNotChangeRunnableState) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::Object* obj = new Runtime::Object();
    auto& table = ObjectMonitorTable::instance();

    EXPECT_EQ(owner->getThreadState(), Runtime::JavaThreadState::Runnable);
    EXPECT_THROW(table.monitorWait(obj, owner, 1), std::runtime_error);
    EXPECT_EQ(owner->getThreadState(), Runtime::JavaThreadState::Runnable);
}
