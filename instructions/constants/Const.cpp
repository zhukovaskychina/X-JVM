//
// Created by zhukovasky on 2020/8/13.
//

#include "Const.h"
namespace Instruction{

    Const::~Const() {

    }

    void ACONST_NULL::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
      //  NoOperationInstruction::fetchOperands(byteCodeReader);
    }

    void ACONST_NULL::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushSlots(nullptr);
    }

    void DCONST_0::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushDouble(0);
    }

    void DCONST_1::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushDouble(1);
    }

    void FCONST_0::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushFloat(0);
    }

    void FCONST_1::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        NoOperationInstruction::fetchOperands(byteCodeReader);
    }

    void FCONST_1::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushFloat(1);
    }

    void FCONST_2::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushFloat(2);
    }

    void ICONST_M1::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(-1);
    }

    void ICONST_0::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(0);
    }

    void ICONST_1::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(1);
    }

    void ICONST_2::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(2);
    }

    void ICONST_3::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(3);
    }

    void ICONST_4::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(4);
    }

    void ICONST_5::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(5);
    }

    void LCONST_0::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushLong(0);
    }

    void LCONST_1::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushLong(1);
    }
};