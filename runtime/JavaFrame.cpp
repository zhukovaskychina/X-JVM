//
// Created by zhukovasky on 2020/8/10.
//

#include "JavaFrame.h"

namespace Runtime{

    JavaFrame::~JavaFrame() {
        delete(this->slots);
        delete(this->operandStack);
    }

    //从局部变量表加载常量,压入到操作数栈
    void JavaFrame::load(u1 idx) {
        this->operandStack->pushSlots(&(this->slots[idx]));
    }

    //从操作数栈出栈，存储到局部变量表
    void JavaFrame::store(u1 idx) {

    }


    long JavaFrame::getPc() const {
        return pc;
    }

    OperandStack *JavaFrame::getOperandStack() {
        return this->operandStack;
    }

    Heap::Method *JavaFrame::getMethod() const {
        return method;
    }

    void JavaFrame::setMethod(Heap::Method *method) {
        JavaFrame::method = method;
    }

    void JavaFrame::setPc(long pc) {
        this->pc=pc;
    }

    JavaFrame::JavaFrame(JavaThread *currentThread, Heap::Method *method) {
        this->method=method;
        this->operandStack=OperandStack::newOperandStack(method->getMaxStack());
        this->javaThread=currentThread;
        this->localVariableTables=new LocalVariableTables(method->getMaxLocal());
        this->nextPc=0;
        this->pc=0;
    }

    LocalVariableTables *JavaFrame::getLocalVariableTables() {
        return this->localVariableTables;
    }

    JavaThread *JavaFrame::getJavaThread() const {
        return javaThread;
    }

    void JavaFrame::setJavaThread(JavaThread *javaThread) {
        JavaFrame::javaThread = javaThread;
    }

    long JavaFrame::getNextPc() const {
        return nextPc;
    }

    void JavaFrame::setNextPc(long nextPc) {
        JavaFrame::nextPc = nextPc;
    }

    void JavaFrame::revertNextPc() {
        int pc=this->getJavaThread()->getPcAddress();
        this->nextPc=pc;
    }
}