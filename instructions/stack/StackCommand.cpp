//
// Created by zhukovasky on 2020/8/14.
//

#include "StackCommand.h"
namespace Instruction{

    void Pop::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->popSlots();
    }
    //double和long变量在操作数栈中占据两个位置，需要用pop两次
    void Pop2::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->popSlots();
    }

    void Dup::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots* slots=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(slots);
        javaFrame->getOperandStack()->pushSlots(slots);
    }

    void Dup2::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots *first=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *second=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
    }

    void DupX1::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots *first=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *second=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(first);
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
    }

    void DupX2::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots *first=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *second=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *third=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(first);
        javaFrame->getOperandStack()->pushSlots(third);
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
    }

    void Dup2X1::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots *first=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *second=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *third=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
        javaFrame->getOperandStack()->pushSlots(third);
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
    }
    void Dup2X2::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots *first=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *second=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *third=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *fourth=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
        javaFrame->getOperandStack()->pushSlots(fourth);
        javaFrame->getOperandStack()->pushSlots(third);
        javaFrame->getOperandStack()->pushSlots(second);
        javaFrame->getOperandStack()->pushSlots(first);
    }

    void Swap::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Slots *first=javaFrame->getOperandStack()->popSlots();
        Runtime::Slots *second=javaFrame->getOperandStack()->popSlots();
        javaFrame->getOperandStack()->pushSlots(first);
        javaFrame->getOperandStack()->pushSlots(second);
    }
};