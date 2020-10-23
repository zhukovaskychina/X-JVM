//
// Created by zhukovasky on 2020/8/12.
//

#include "JavaThread.h"

namespace Runtime{


    JavaThread::~JavaThread() {

    }

    int JavaThread::getStackSize() const {
        return stackSize;
    }

    void JavaThread::setStackSize(int stackSize) {
        JavaThread::stackSize = stackSize;
    }

    long JavaThread::getPcAddress() const {
        return pcAddress;
    }

    void JavaThread::setPcAddress(long pcAddress) {
        JavaThread::pcAddress = pcAddress;
    }

    RuntimeStack *JavaThread::getJavaStack() const {
        return javaStack;
    }

    void JavaThread::setJavaStack(RuntimeStack *javaStack) {
        JavaThread::javaStack = javaStack;
    }

//    const Object &JavaThread::getJThread() const {
//        return jThread;
//    }
//
//    void JavaThread::setJThread(const Object &jThread) {
//        JavaThread::jThread = jThread;
//    }

    JavaFrame *JavaThread::getJavaFrame() const {
        return javaFrame;
    }

    void JavaThread::setJavaFrame(JavaFrame *javaFrame) {
        JavaThread::javaFrame = javaFrame;
    }

    Runtime::JavaThread *JavaThread::newThread(int stackSize) {
        RuntimeStack *runtimeStack=new RuntimeStack(stackSize);
        Runtime::JavaThread *jthread=new Runtime::JavaThread();
       // Runtime::JavaFrame *javaFrame=new Runtime::JavaFrame();
        jthread->setJavaStack(runtimeStack);
       // jthread->setJavaFrame(javaFrame);

        return jthread;
    }

    void JavaThread::pushJavaFrame(JavaFrame *javaFrame) {
        this->javaStack->push(javaFrame);
    }

    JavaFrame *JavaThread::popJavaFrame() {
        return this->javaStack->pop();
    }

    JavaFrame *JavaThread::currentFrame() {
        return this->javaStack->top();
    }

    JavaThread::JavaThread() {

    }
}