//
// Created by zhukovasky on 2020/8/13.
//

#include "BiSiPush.h"
namespace Instruction{
    void BiPush::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        u1 value=byteCodeReader->readU1();
        this->value=value;
    }

    void BiPush::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(this->value);
    }

    void SiPush::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
       u2 value=byteCodeReader->readU2();
       this->value=value;
    }

    void SiPush::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getOperandStack()->pushInt(this->value);
    }
}