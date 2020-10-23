//
// Created by zhukovasky on 2020/9/8.
//

#include "AStore.h"
namespace Instruction{
    void IAStore::execute(Runtime::JavaFrame *javaFrame) {
        int value=javaFrame->getOperandStack()->popInt();
        int index=javaFrame->getOperandStack()->popInt();
        Runtime::Object* object=javaFrame->getOperandStack()->popObject();
        int* intArrays=(int*)object->getData();
        intArrays[index]=value;
    }

}
