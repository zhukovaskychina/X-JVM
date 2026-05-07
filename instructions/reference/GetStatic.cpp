//
// Created by zhukovasky on 2020/8/25.
//

#include "GetStatic.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../utils/StringUtils.h"

#include <stdexcept>

namespace Instruction{
    void GetStatic::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass *currentJavaClass=javaFrame->getMethod()->getJavaClass();


        Runtime::Heap::FieldRef *fieldRef=currentJavaClass->getRuntimeConstantsPool()->getIndex(this->getIndex())->getFieldRef();

        Runtime::Heap::FieldInfo *fieldInfo=fieldRef->resolvedFields();

        Runtime::JavaClass *fieldJavaClass=fieldInfo->getJavaClass();

        if(!fieldJavaClass->initStarted()){
            javaFrame->revertNextPc();
            fieldJavaClass->initJavaClass(javaFrame->getJavaThread(),fieldJavaClass);
            return;;
        }


        if(!fieldInfo->isStatic()){
            throw std::runtime_error("GetStatic: IncompatibleClassChangeError (non-static field)");
        }

        std::string fieldDescriptor=fieldInfo->getDescriptor();
        u1 slotId=fieldInfo->getSlotId();
        std::vector<Runtime::Slots*> slots=fieldJavaClass->getStaticVars();
        if (static_cast<size_t>(slotId) >= slots.size() || !slots[slotId]) {
            throw std::runtime_error("GetStatic: static slot not allocated");
        }
        Runtime::Slots* slot = slots[slotId];

        if(fieldDescriptor=="Z"||fieldDescriptor=="B"||fieldDescriptor=="C"||fieldDescriptor=="S"||fieldDescriptor=="I"){
            javaFrame->getOperandStack()->pushInt(slot->getNums());
        } else if(fieldDescriptor=="F"){
            javaFrame->getOperandStack()->pushFloat(slot->getFloatValue());
        } else if(fieldDescriptor=="J"){
            javaFrame->getOperandStack()->pushLong(slot->getLongValue());
        } else if(fieldDescriptor=="D"){
            javaFrame->getOperandStack()->pushDouble(slot->getDoubleValue());
        } else if(Utils::StringUtils::startsWith(fieldDescriptor,"L") || Utils::StringUtils::startsWith(fieldDescriptor,"[")){
            javaFrame->getOperandStack()->pushRef(slot->getRefs());
        }

    }
}