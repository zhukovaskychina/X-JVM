//
// Created by zhukovasky on 2020/8/25.
//

#include "PutStatic.h"
#include "../../runtime/heap/JavaClass.h"
namespace Instruction{
    void PutStatic::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Heap::Method *currentMethod=javaFrame->getMethod();
        Runtime::JavaClass* currentJavaClass=currentMethod->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool *runtimeConstantsPool=currentJavaClass->getRuntimeConstantsPool();
        Runtime::Heap::FieldRef* fieldRefs=runtimeConstantsPool->getIndex(this->getIndex())->getFieldRef();
        Runtime::Heap::FieldInfo *field = fieldRefs->resolvedFields();
        Runtime::JavaClass *javaClassField=field->getJavaClass();


        if(!field->isStatic()){
            //
        }

        if(field->isFinal()){
            if(currentJavaClass!=javaClassField||currentMethod->getName()!="<clinit>"){

            }
        }

        std::string descriptor=field->getDescriptor();

        u1 slotId = field->getSlotId();

//        Runtime::Slots *slots = javaClassField->getStaticVars();
//
//
//        if(descriptor=="Z"||descriptor=="B"||descriptor=="C"||descriptor=="S"||descriptor=="I"){
//            slots[slotId].setNums(javaFrame->getOperandStack()->popInt());
//        }
//
//        if(descriptor=="F"){
//            slots[slotId].setFloatValue(javaFrame->getOperandStack()->popFloat());
//        }
//        if(descriptor=="J"){
//            slots[slotId].setNums(javaFrame->getOperandStack()->popLong());
//        }
//        if(descriptor=="D"){
//            slots[slotId].setDoubleValue(javaFrame->getOperandStack()->popDouble());
//        }
//        if(descriptor=="L"){
//            slots[slotId].setRefs(javaFrame->getOperandStack()->popObject());
//        }
    }
}