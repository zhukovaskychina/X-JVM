//
// Created by zhukovasky on 2020/8/25.
//

#include "GetStatic.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../utils/StringUtils.h"

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
            cerr<<"java.lang.IncompatibleClassChangeError"<<endl;
            exit(0);
        }

        std::string fieldDescriptor=fieldInfo->getDescriptor();
        u1 slotId=fieldInfo->getSlotId();
        std::vector<Runtime::Slots*> slots=fieldJavaClass->getStaticVars();


        if(fieldDescriptor=="Z"||fieldDescriptor=="B"||fieldDescriptor=="C"||fieldDescriptor=="S"||fieldDescriptor=="I"){
            javaFrame->getOperandStack()->pushInt(slots[this->getIndex()]->getNums());
        }
        if(fieldDescriptor=="F"){
            javaFrame->getOperandStack()->pushFloat(slots[this->getIndex()]->getFloatValue());
        }
        if(fieldDescriptor=="J"){

        }
        if(fieldDescriptor=="D"){

        }
        if(Utils::StringUtils::startsWith(fieldDescriptor,"L")){
            Runtime::Object* object=new Runtime::Object();
            object->setFields(slots[slotId]);
            javaFrame->getOperandStack()->pushRef(object);
        }

    }
}