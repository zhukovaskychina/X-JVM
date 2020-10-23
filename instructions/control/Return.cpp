//
// Created by zhukovasky on 2020/8/31.
//

#include "Return.h"
namespace Instruction{
    void ReturnInst::execute(Runtime::JavaFrame *javaFrame) {
        javaFrame->getJavaThread()->popJavaFrame();
    }

    ReturnInst::ReturnInst() {
      //  cout<<"初始化Return指令"<<endl;
    }

    ReturnInst::~ReturnInst() {

    }


    void IReturn::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaThread *jthread=javaFrame->getJavaThread();
        Runtime::JavaFrame* currentJavaFrame=jthread->popJavaFrame();
        Runtime::JavaFrame* invokeFrame=jthread->currentFrame();
        int value=currentJavaFrame->getOperandStack()->popInt();
        invokeFrame->getOperandStack()->pushInt(value);
    }

    void FReturn::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaThread *jthread=javaFrame->getJavaThread();
        Runtime::JavaFrame* currentJavaFrame=jthread->popJavaFrame();
        Runtime::JavaFrame* invokeFrame=jthread->currentFrame();
        float value=currentJavaFrame->getOperandStack()->popFloat();
        invokeFrame->getOperandStack()->pushFloat(value);
    }

    void DReturn::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaThread *jthread=javaFrame->getJavaThread();
        Runtime::JavaFrame* currentJavaFrame=jthread->popJavaFrame();
        Runtime::JavaFrame* invokeFrame=jthread->currentFrame();
        double value=currentJavaFrame->getOperandStack()->popDouble();
        invokeFrame->getOperandStack()->pushDouble(value);
    }

    void AReturn::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaThread *jthread=javaFrame->getJavaThread();
        Runtime::JavaFrame* currentJavaFrame=jthread->popJavaFrame();
        Runtime::JavaFrame* invokeFrame=jthread->currentFrame();
        Runtime::Object* value=currentJavaFrame->getOperandStack()->popObject();
        invokeFrame->getOperandStack()->pushRef(value);
    }

    void LReturn::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaThread *jthread=javaFrame->getJavaThread();
        Runtime::JavaFrame* currentJavaFrame=jthread->popJavaFrame();
        Runtime::JavaFrame* invokeFrame=jthread->currentFrame();
        long value=currentJavaFrame->getOperandStack()->popLong();
        invokeFrame->getOperandStack()->pushLong(value);
    }
}