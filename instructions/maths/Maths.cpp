//
// Created by zhukovasky on 2020/8/14.
//

#include <cmath>
#include "Maths.h"
namespace Instruction{
    void IRem::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        int result=first%second;
        javaFrame->getOperandStack()->pushInt(result);
    }
    void LRem::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        long result=first%second;
        javaFrame->getOperandStack()->pushLong(result);
    }

    void FRem::execute(Runtime::JavaFrame *javaFrame) {
        float first=javaFrame->getOperandStack()->popFloat();
        float second=javaFrame->getOperandStack()->popFloat();

        javaFrame->getOperandStack()->pushInt(fmodf64(first,second));
    }
    void DRem::execute(Runtime::JavaFrame *javaFrame) {
        double first=javaFrame->getOperandStack()->popDouble();
        double second=javaFrame->getOperandStack()->popDouble();
        int result=fmodf64(first,second);
        javaFrame->getOperandStack()->pushInt(result);
    }

    void IDiv::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        int result=first/second;
        javaFrame->getOperandStack()->pushInt(result);
    }
    void FDiv::execute(Runtime::JavaFrame *javaFrame) {
        float first=javaFrame->getOperandStack()->popFloat();
        float second=javaFrame->getOperandStack()->popFloat();
        float result=first/second;
        javaFrame->getOperandStack()->pushInt(result);
    }
    void LDiv::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        long result=first/second;
        javaFrame->getOperandStack()->pushLong(result);
    }
    void DDiv::execute(Runtime::JavaFrame *javaFrame) {
        double first=javaFrame->getOperandStack()->popLong();
        double second=javaFrame->getOperandStack()->popLong();
        int result=first/second;
        javaFrame->getOperandStack()->pushInt(result);
    }

    void FAdd::execute(Runtime::JavaFrame *javaFrame) {
        float first=javaFrame->getOperandStack()->popFloat();
        float second=javaFrame->getOperandStack()->popFloat();
        float result=first+second;
        javaFrame->getOperandStack()->pushFloat(result);
    }
    void DAdd::execute(Runtime::JavaFrame *javaFrame) {
        double first=javaFrame->getOperandStack()->popDouble();
        double second=javaFrame->getOperandStack()->popDouble();
        double result=first+second;
        javaFrame->getOperandStack()->pushDouble(result);
    }
    void IAdd::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        int result=first+second;
        javaFrame->getOperandStack()->pushFloat(result);
    }
    void LAdd::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        long result=first+second;
        javaFrame->getOperandStack()->pushLong(result);
    }

    void ISub::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        int result=first-second;
        javaFrame->getOperandStack()->pushFloat(result);
    };
    void FSub::execute(Runtime::JavaFrame *javaFrame) {
        float first=javaFrame->getOperandStack()->popFloat();
        float second=javaFrame->getOperandStack()->popFloat();
        float result=first-second;
        javaFrame->getOperandStack()->pushFloat(result);
    }
    void DSub::execute(Runtime::JavaFrame *javaFrame) {
        double first=javaFrame->getOperandStack()->popDouble();
        double second=javaFrame->getOperandStack()->popDouble();
        double result=first-second;
        javaFrame->getOperandStack()->pushDouble(result);
    }
    void LSub::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        long result=first-second;
        javaFrame->getOperandStack()->pushLong(result);
    }

    void IMul::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        int result=first*second;
        javaFrame->getOperandStack()->pushFloat(result);
    }
    void FMul::execute(Runtime::JavaFrame *javaFrame) {
        float first=javaFrame->getOperandStack()->popFloat();
        float second=javaFrame->getOperandStack()->popFloat();
        float result=first*second;
        javaFrame->getOperandStack()->pushFloat(result);
    }
    void DMul::execute(Runtime::JavaFrame *javaFrame) {
        double first=javaFrame->getOperandStack()->popDouble();
        double second=javaFrame->getOperandStack()->popDouble();
        double result=first*second;
        javaFrame->getOperandStack()->pushDouble(result);
    }
    void LMul::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        long result=first*second;
        javaFrame->getOperandStack()->pushLong(result);
    }

    void INeg::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        javaFrame->getOperandStack()->pushInt(-first);
    }
    void DNeg::execute(Runtime::JavaFrame *javaFrame) {
        double first=javaFrame->getOperandStack()->popDouble();
        javaFrame->getOperandStack()->pushDouble(-first);
    }
    void FNeg::execute(Runtime::JavaFrame *javaFrame) {
        float first=javaFrame->getOperandStack()->popFloat();
        javaFrame->getOperandStack()->pushFloat(-first);
    }
    void LNeg::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        javaFrame->getOperandStack()->pushLong(-first);
    }

    void LXor::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        javaFrame->getOperandStack()->pushInt(first^second);
    }
    void IXor::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        javaFrame->getOperandStack()->pushInt(first^second);
    }

    void Ior::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        javaFrame->getOperandStack()->pushInt(first|second);
    }

    void Lor::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        javaFrame->getOperandStack()->pushInt(first|second);
    }

    void LAnd::execute(Runtime::JavaFrame *javaFrame) {
        long first=javaFrame->getOperandStack()->popLong();
        long second=javaFrame->getOperandStack()->popLong();
        javaFrame->getOperandStack()->pushLong(first&second);
    }

    void IAnd::execute(Runtime::JavaFrame *javaFrame) {
        int first=javaFrame->getOperandStack()->popInt();
        int second=javaFrame->getOperandStack()->popInt();
        javaFrame->getOperandStack()->pushInt(first&second);
    }

    //int 算术右位移
    void ISHR::execute(Runtime::JavaFrame *javaFrame) {

    }
    //int 逻辑右位移
    void IUSHR::execute(Runtime::JavaFrame *javaFrame) {

    }
    //int 左位移
    void ISHL::execute(Runtime::JavaFrame *javaFrame) {
        int value2=javaFrame->getOperandStack()->popInt();
        int value1=javaFrame->getOperandStack()->popInt();
        int r=value2&0x1f;
        javaFrame->getOperandStack()->pushInt(value1<<r);
    }
    void LSHR::execute(Runtime::JavaFrame *javaFrame) {

    }

    void LUSHR::execute(Runtime::JavaFrame *javaFrame) {

    }

    void LSHL::execute(Runtime::JavaFrame *javaFrame) {

    }

    void IInc::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        this->index=byteCodeReader->readU1();
        this->constants=byteCodeReader->readU1();
    }

    void IInc::execute(Runtime::JavaFrame *javaFrame) {
        int value=javaFrame->getLocalVariableTables()->getInt(this->index);
        value=value+this->constants;
        javaFrame->getLocalVariableTables()->setInt(this->index,value);
    }
}