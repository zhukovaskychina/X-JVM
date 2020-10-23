//
// Created by zhukovasky on 2020/9/8.
//

#include "Aload.h"

namespace Instruction{

    void AAload::execute(Runtime::JavaFrame *javaFrame) {
        NoOperationInstruction::execute(javaFrame);
    }

    void BAload::execute(Runtime::JavaFrame *javaFrame) {
        NoOperationInstruction::execute(javaFrame);
    }

    void CAload::execute(Runtime::JavaFrame *javaFrame) {
        NoOperationInstruction::execute(javaFrame);
    }

    void DAload::execute(Runtime::JavaFrame *javaFrame) {
        NoOperationInstruction::execute(javaFrame);
    }

    void FAload::execute(Runtime::JavaFrame *javaFrame) {
        NoOperationInstruction::execute(javaFrame);
    }

    void IAload::execute(Runtime::JavaFrame *javaFrame) {
        int index=javaFrame->getOperandStack()->popInt();
        Runtime::Object* object=javaFrame->getOperandStack()->popObject();
        int* intarrays=(int*)object->getData();
        javaFrame->getOperandStack()->pushInt(intarrays[index]);
    }

    void LAload::execute(Runtime::JavaFrame *javaFrame) {
        NoOperationInstruction::execute(javaFrame);
    }
};