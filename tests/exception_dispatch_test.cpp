#include <gtest/gtest.h>

#include "runtime/ExceptionDispatch.h"
#include "runtime/JavaFrame.h"
#include "runtime/JavaThread.h"
#include "runtime/Object.h"
#include "runtime/heap/ClassMember.h"

namespace {

Runtime::Heap::Method* makeMethodWithHandler(u2 startPc, u2 endPc, u2 handlerPc) {
    auto* m = new Runtime::Heap::Method();
    m->setMaxStack(8);
    m->setMaxLocal(2);
    m->addExceptionHandler({startPc, endPc, handlerPc, 0});
    return m;
}

} // namespace

TEST(ExceptionDispatch, HandlerInCurrentFrameConsumesException) {
    Runtime::JavaThread* thread = Runtime::JavaThread::newThread(16);
    ASSERT_NE(thread, nullptr);

    Runtime::Heap::Method* method = makeMethodWithHandler(10, 20, 88);
    auto* frame = new Runtime::JavaFrame(thread, method);
    frame->setCurrentInsnBegin(12);
    thread->pushJavaFrame(frame);

    Runtime::Object* marker = new Runtime::Object();
    Runtime::Object* ex = new Runtime::Object();
    ex->setJavaClass(reinterpret_cast<Runtime::JavaClass*>(0x1));
    frame->getOperandStack()->pushRef(marker);

    ASSERT_TRUE(Runtime::tryDispatchException(thread, ex));
    EXPECT_EQ(thread->currentFrame(), frame);
    EXPECT_EQ(frame->getNextPc(), 88);
    EXPECT_EQ(frame->getOperandStack()->popObject(), ex);

    // 说明：JavaFrame 析构当前会触发历史生命周期问题，这里避免释放 frame/thread。
    // 测试进程短生命周期下允许这类测试对象泄漏，防止误报崩溃。
    delete marker;
    delete ex;
}

TEST(ExceptionDispatch, UnwindToCallerUsesInvokeSitePc) {
    Runtime::JavaThread* thread = Runtime::JavaThread::newThread(16);
    ASSERT_NE(thread, nullptr);

    auto* callerMethod = makeMethodWithHandler(30, 40, 123);
    auto* caller = new Runtime::JavaFrame(thread, callerMethod);
    caller->setCurrentInsnBegin(30);
    thread->pushJavaFrame(caller);

    auto* calleeMethod = new Runtime::Heap::Method();
    calleeMethod->setMaxStack(4);
    calleeMethod->setMaxLocal(1);
    auto* callee = new Runtime::JavaFrame(thread, calleeMethod);
    callee->setCurrentInsnBegin(7);
    callee->setInvokeSitePc(31);
    thread->pushJavaFrame(callee);

    Runtime::Object* ex = new Runtime::Object();
    ex->setJavaClass(reinterpret_cast<Runtime::JavaClass*>(0x2));

    ASSERT_TRUE(Runtime::tryDispatchException(thread, ex));
    EXPECT_EQ(thread->currentFrame(), caller);
    EXPECT_EQ(caller->getNextPc(), 123);
    EXPECT_EQ(caller->getOperandStack()->popObject(), ex);

    delete ex;
}

TEST(ExceptionDispatch, HandlerClearsOperandStackBeforePushException) {
    Runtime::JavaThread* thread = Runtime::JavaThread::newThread(16);
    ASSERT_NE(thread, nullptr);

    Runtime::Heap::Method* method = makeMethodWithHandler(1, 100, 50);
    auto* frame = new Runtime::JavaFrame(thread, method);
    frame->setCurrentInsnBegin(5);
    thread->pushJavaFrame(frame);

    frame->getOperandStack()->pushInt(42);
    Runtime::Object* ex = new Runtime::Object();
    ex->setJavaClass(reinterpret_cast<Runtime::JavaClass*>(0x3));

    ASSERT_TRUE(Runtime::tryDispatchException(thread, ex));
    EXPECT_EQ(frame->getNextPc(), 50);
    EXPECT_EQ(frame->getOperandStack()->popObject(), ex);

    delete ex;
}
