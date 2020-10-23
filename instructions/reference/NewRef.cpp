//
// Created by zhukovasky on 2020/8/25.
//

#include "NewRef.h"

#include "../../runtime/heap/JavaClass.h"
using namespace Runtime;
namespace Instruction{
    void NewRef::execute(Runtime::JavaFrame *javaFrame) {
        Heap::Method *method=javaFrame->getMethod();
        JavaClass* javaClass=method->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool *runtimeConstantsPool=javaClass->getRuntimeConstantsPool();
        Heap::ClassRef *pClassRef=runtimeConstantsPool->getIndex(this->getIndex())->getClassRef();
        JavaClass *javaClassInstance=pClassRef->resolvedJavaClass();
        if(javaClassInstance->isAbstract()||javaClassInstance->isInterface()){
            //抛出异常
            cerr<<("InCompatibleException")<<endl;
        }
        Object *object=javaClassInstance->createNewJavaObjectInstance();
        javaFrame->getOperandStack()->pushRef(object);

    }
}