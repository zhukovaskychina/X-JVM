//
// Created by zhukovasky on 2020/9/8.
//

#include "Aload.h"
#include "../../runtime/Object.h"

namespace Instruction{

    void AAload::execute(Runtime::JavaFrame *javaFrame) {
        int index = javaFrame->getOperandStack()->popInt();
        Runtime::Object *arr = javaFrame->getOperandStack()->popObject();
        if (!arr || index < 0 || index >= arr->getArrayLength()) {
            javaFrame->getOperandStack()->pushRef(nullptr);
            return;
        }
        auto **refs = reinterpret_cast<Runtime::Object **>(arr->getData());
        if (!refs) {
            javaFrame->getOperandStack()->pushRef(nullptr);
            return;
        }
        javaFrame->getOperandStack()->pushRef(refs[index]);
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