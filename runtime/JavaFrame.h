//
// Created by zhukovasky on 2020/8/10.
//

#ifndef JAVAP_JAVAFRAME_H
#define JAVAP_JAVAFRAME_H


#include "iostream"
#include <stack>
#include "./JavaType.h"
#include "Slots.h"
#include "OperandStack.h"
#include "../common/Internal.h"
#include "heap/ClassMember.h"
#include "JavaThread.h"
#include "LocalVariableTables.h"
#include "heap/JavaHeap.h"
namespace Runtime{
    class JavaThread;
    class JavaFrame {

    public:


        JavaFrame(JavaThread* currentThread,Heap::Method *method);

        void load(u1 idx);

        void store(u1 idx);

        virtual ~JavaFrame();

        OperandStack* getOperandStack();

        LocalVariableTables* getLocalVariableTables();

        Heap::Method *getMethod() const;

        void setMethod(Heap::Method *method);

        long getPc() const;

        void setPc(long pc);

        JavaThread *getJavaThread() const;

        void setJavaThread(JavaThread *javaThread);

        long getNextPc() const;

        void setNextPc(long nextPc);


        void revertNextPc();

    private:
        //操作数栈
        OperandStack* operandStack;
        //局部变量表

        //前一个frame
        JavaFrame* lowerFrame;

        Slots *slots;

        //构建本地变量表
        LocalVariableTables* localVariableTables;

        Heap::Method *method;

        long pc;

        JavaThread* javaThread;

        long nextPc;
    };

}



#endif //JAVAP_JAVAFRAME_H
