//
// Created by zhukovasky on 2020/9/8.
//

#include "AStore.h"
#include "../../runtime/Object.h"
#include "../../runtime/ClassLoader.h"
#include "../../runtime/TypeHelper.h"
#include "../../runtime/heap/JavaClass.h"

namespace Instruction{

    void AAStore::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Object *value = javaFrame->getOperandStack()->popObject();
        int index = javaFrame->getOperandStack()->popInt();
        Runtime::Object *arr = javaFrame->getOperandStack()->popObject();
        if (!arr || index < 0 || index >= arr->getArrayLength()) {
            return;
        }
        auto **refs = reinterpret_cast<Runtime::Object **>(arr->getData());
        if (!refs) {
            return;
        }
        Runtime::JavaClass *arrCl = arr->getJavaClass();
        Runtime::ClassLoader *cl = javaFrame->getMethod() && javaFrame->getMethod()->getJavaClass()
                                       ? javaFrame->getMethod()->getJavaClass()->getClassLoader()
                                       : nullptr;
        if (value && arrCl && cl) {
            Runtime::JavaClass *comp = Runtime::arrayComponentJavaClass(arrCl, cl);
            if (comp && value->getJavaClass() &&
                !Runtime::isJvmAssignable(value->getJavaClass(), comp, cl)) {
                return;
            }
        }
        refs[index] = value;
    }

    void IAStore::execute(Runtime::JavaFrame *javaFrame) {
        int value=javaFrame->getOperandStack()->popInt();
        int index=javaFrame->getOperandStack()->popInt();
        Runtime::Object* object=javaFrame->getOperandStack()->popObject();
        int* intArrays=(int*)object->getData();
        intArrays[index]=value;
    }

}
