//
// Created by zhukovasky on 2020/8/31.
//

#include "Comparisons.h"

namespace Instruction {

static void branchIf(Runtime::JavaFrame* javaFrame, bool cond, int offset) {
    if (!cond) {
        return;
    }
    const int pc = static_cast<int>(javaFrame->getJavaThread()->getPcAddress());
    javaFrame->getJavaThread()->setPcAddress(static_cast<long>(pc + offset));
}

void IfIcmpeq::execute(Runtime::JavaFrame* javaFrame) {
    const int v2 = javaFrame->getOperandStack()->popInt();
    const int v1 = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v1 == v2, this->getOffset());
}

void IfIcmpne::execute(Runtime::JavaFrame* javaFrame) {
    const int v2 = javaFrame->getOperandStack()->popInt();
    const int v1 = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v1 != v2, this->getOffset());
}

void IfIcmplt::execute(Runtime::JavaFrame* javaFrame) {
    const int v2 = javaFrame->getOperandStack()->popInt();
    const int v1 = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v1 < v2, this->getOffset());
}

void IfIcmpge::execute(Runtime::JavaFrame* javaFrame) {
    const int v2 = javaFrame->getOperandStack()->popInt();
    const int v1 = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v1 >= v2, this->getOffset());
}

void IfIcmpgt::execute(Runtime::JavaFrame* javaFrame) {
    const int v2 = javaFrame->getOperandStack()->popInt();
    const int v1 = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v1 > v2, this->getOffset());
}

void IfIcmple::execute(Runtime::JavaFrame* javaFrame) {
    const int v2 = javaFrame->getOperandStack()->popInt();
    const int v1 = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v1 <= v2, this->getOffset());
}

void IfAcmpeq::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Object* v2 = javaFrame->getOperandStack()->popObject();
    Runtime::Object* v1 = javaFrame->getOperandStack()->popObject();
    branchIf(javaFrame, v1 == v2, this->getOffset());
}

void IfAcmpne::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Object* v2 = javaFrame->getOperandStack()->popObject();
    Runtime::Object* v1 = javaFrame->getOperandStack()->popObject();
    branchIf(javaFrame, v1 != v2, this->getOffset());
}

void IfEq::execute(Runtime::JavaFrame* javaFrame) {
    const int v = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v == 0, this->getOffset());
}

void IfNe::execute(Runtime::JavaFrame* javaFrame) {
    const int v = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v != 0, this->getOffset());
}

void IfLt::execute(Runtime::JavaFrame* javaFrame) {
    const int v = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v < 0, this->getOffset());
}

void IfGe::execute(Runtime::JavaFrame* javaFrame) {
    const int v = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v >= 0, this->getOffset());
}

void IfGt::execute(Runtime::JavaFrame* javaFrame) {
    const int v = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v > 0, this->getOffset());
}

void IfLe::execute(Runtime::JavaFrame* javaFrame) {
    const int v = javaFrame->getOperandStack()->popInt();
    branchIf(javaFrame, v <= 0, this->getOffset());
}

void IfNull::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Object* v = javaFrame->getOperandStack()->popObject();
    branchIf(javaFrame, v == nullptr, this->getOffset());
}

void IfNonNull::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Object* v = javaFrame->getOperandStack()->popObject();
    branchIf(javaFrame, v != nullptr, this->getOffset());
}

} // namespace Instruction
