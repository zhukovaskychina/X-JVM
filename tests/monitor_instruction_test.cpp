#include <gtest/gtest.h>

#include "instructions/control/Monitor.h"
#include "runtime/JavaFrame.h"
#include "runtime/JavaThread.h"
#include "runtime/Object.h"
#include "runtime/threading/ObjectMonitor.h"
#include "runtime/heap/ClassMember.h"

namespace {

Runtime::JavaFrame* newFrame(Runtime::JavaThread* thread) {
    auto* method = new Runtime::Heap::Method();
    method->setMaxStack(8);
    method->setMaxLocal(2);
    auto* frame = new Runtime::JavaFrame(thread, method);
    thread->pushJavaFrame(frame);
    return frame;
}

} // namespace

class MonitorInstructionTest : public ::testing::Test {
protected:
    void SetUp() override {
        Runtime::Threading::ObjectMonitorTable::instance().resetForTest();
    }
};

TEST_F(MonitorInstructionTest, ReentrantEnterAndExitWorks) {
    Runtime::JavaThread* thread = Runtime::JavaThread::newThread(8);
    ASSERT_NE(thread, nullptr);
    Runtime::JavaFrame* frame = newFrame(thread);
    ASSERT_NE(frame, nullptr);

    Runtime::Object* obj = new Runtime::Object();
    Instruction::MonitorEnter enter;
    Instruction::MonitorExit exit;

    frame->getOperandStack()->pushRef(obj);
    enter.execute(frame);
    EXPECT_EQ(Runtime::Threading::ObjectMonitorTable::instance().recursionForTest(obj), 1);

    frame->getOperandStack()->pushRef(obj);
    enter.execute(frame);
    EXPECT_EQ(Runtime::Threading::ObjectMonitorTable::instance().recursionForTest(obj), 2);

    frame->getOperandStack()->pushRef(obj);
    exit.execute(frame);
    EXPECT_EQ(Runtime::Threading::ObjectMonitorTable::instance().recursionForTest(obj), 1);

    frame->getOperandStack()->pushRef(obj);
    exit.execute(frame);
    EXPECT_EQ(Runtime::Threading::ObjectMonitorTable::instance().recursionForTest(obj), 0);
}

TEST_F(MonitorInstructionTest, ExitByNonOwnerThrows) {
    Runtime::JavaThread* owner = Runtime::JavaThread::newThread(8);
    Runtime::JavaThread* other = Runtime::JavaThread::newThread(8);
    ASSERT_NE(owner, nullptr);
    ASSERT_NE(other, nullptr);

    Runtime::JavaFrame* ownerFrame = newFrame(owner);
    Runtime::JavaFrame* otherFrame = newFrame(other);
    ASSERT_NE(ownerFrame, nullptr);
    ASSERT_NE(otherFrame, nullptr);

    Runtime::Object* obj = new Runtime::Object();
    Instruction::MonitorEnter enter;
    Instruction::MonitorExit exit;

    ownerFrame->getOperandStack()->pushRef(obj);
    enter.execute(ownerFrame);

    otherFrame->getOperandStack()->pushRef(obj);
    EXPECT_THROW(exit.execute(otherFrame), std::runtime_error);
}

TEST_F(MonitorInstructionTest, NullRefThrowsNpeStyleError) {
    Runtime::JavaThread* thread = Runtime::JavaThread::newThread(8);
    Runtime::JavaFrame* frame = newFrame(thread);
    ASSERT_NE(frame, nullptr);

    Instruction::MonitorEnter enter;
    frame->getOperandStack()->pushRef(nullptr);
    EXPECT_THROW(enter.execute(frame), std::runtime_error);
}
