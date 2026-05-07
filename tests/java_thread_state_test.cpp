#include <gtest/gtest.h>

#include "runtime/JavaThread.h"

TEST(JavaThreadState, NewThreadIsRunnableAfterFactory) {
    Runtime::JavaThread* t = Runtime::JavaThread::newThread(8);
    ASSERT_NE(t, nullptr);
    EXPECT_EQ(t->getThreadState(), Runtime::JavaThreadState::Runnable);
    t->setThreadState(Runtime::JavaThreadState::Terminated);
    EXPECT_EQ(t->getThreadState(), Runtime::JavaThreadState::Terminated);
}

TEST(JavaThreadState, InterruptFlagSupportsClearSemantics) {
    Runtime::JavaThread* t = Runtime::JavaThread::newThread(8);
    ASSERT_NE(t, nullptr);
    EXPECT_FALSE(t->isInterrupted(false));

    t->interrupt();
    EXPECT_TRUE(t->isInterrupted(false));
    EXPECT_TRUE(t->isInterrupted(false));

    EXPECT_TRUE(t->isInterrupted(true));
    EXPECT_FALSE(t->isInterrupted(false));
}
