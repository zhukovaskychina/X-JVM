//
// Created by zhukovasky on 2020/8/31.
//

#include "Comparisons.h"
namespace Instruction{
    void IfIcmpeq::execute(Runtime::JavaFrame *javaFrame) {
        int value1=javaFrame->getOperandStack()->popInt();
        int value2=javaFrame->getOperandStack()->popInt();
        if(value1==value2){
            int pc=javaFrame->getJavaThread()->getPcAddress();
            javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
        }
    }
    void IfIcmpne::execute(Runtime::JavaFrame *javaFrame) {
        int value1=javaFrame->getOperandStack()->popInt();
        int value2=javaFrame->getOperandStack()->popInt();
        if(value1!=value2){
            int pc=javaFrame->getJavaThread()->getPcAddress();
            javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
        }
    }

    void IfIcmplt::execute(Runtime::JavaFrame *javaFrame) {
        int value1=javaFrame->getOperandStack()->popInt();
        int value2=javaFrame->getOperandStack()->popInt();
        if(value1<value2){
            int pc=javaFrame->getJavaThread()->getPcAddress();
            javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
        }
    }

    void IfIcmpge::execute(Runtime::JavaFrame *javaFrame) {
        int value1=javaFrame->getOperandStack()->popInt();
        int value2=javaFrame->getOperandStack()->popInt();
        if(value1>=value2){
            int pc=javaFrame->getJavaThread()->getPcAddress();
            javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
        }
    }

    void IfIcmpgt::execute(Runtime::JavaFrame *javaFrame) {
        int value1=javaFrame->getOperandStack()->popInt();
        int value2=javaFrame->getOperandStack()->popInt();
        if(value1>value2){
            int pc=javaFrame->getJavaThread()->getPcAddress();
            javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
        }
    }

    void IfIcmple::execute(Runtime::JavaFrame *javaFrame) {
        int value1=javaFrame->getOperandStack()->popInt();
        int value2=javaFrame->getOperandStack()->popInt();
        if(value1<=value2){
            int pc=javaFrame->getJavaThread()->getPcAddress();
            javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
        }
    }

    void IfAcmpeq::execute(Runtime::JavaFrame *javaFrame) {

    }

    void IfAcmpne::execute(Runtime::JavaFrame *javaFrame) {

    }
}