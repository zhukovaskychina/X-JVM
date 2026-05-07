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

        /** 当前正在执行指令的起始 PC（在读取操作码之前由解释器设置，用于异常表匹配）。 */
        long getCurrentInsnBegin() const;

        void setCurrentInsnBegin(long beginPc);

        /** 调用本栈帧时在调用者中的 invoke 指令起始 PC（根帧为 0）。 */
        long getInvokeSitePc() const;

        void setInvokeSitePc(long sitePc);

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

        long currentInsnBegin{0};

        long invokeSitePc{0};
    };

}



#endif //JAVAP_JAVAFRAME_H
