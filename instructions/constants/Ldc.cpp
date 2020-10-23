//
// Created by zhukovasky on 2020/9/1.
//

#include "Ldc.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../runtime/heap/ClassMember.h"
#include "../../runtime/heap/ConstantsPoolObject.h"
#include "../../runtime/heap/StringConstantPools.h"
namespace Instruction{
    void LDC::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool *cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::ConstantsPoolObject *constantPoolObject=cp->getIndex(this->getIndex());
        if(constantPoolObject->getConstantType()=="int"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="float"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="double"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="double"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="string"){
           Runtime::Object* obj=Runtime::Heap::JString::getJString()->getJString(javaClass->getClassLoader(),constantPoolObject->getStringValue());
           javaFrame->getOperandStack()->pushRef(obj);
            return;
        }


    }

    void LDC_W::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool *cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::ConstantsPoolObject *constantPoolObject=cp->getIndex(this->getIndex());
        if(constantPoolObject->getConstantType()=="int"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="float"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="double"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="double"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="string"){
            // javaFrame->getOperandStack().push(constantPoolObject->getIntValue());
            return;
        }
    }

    void LDC2_W::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool *cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::ConstantsPoolObject *constantPoolObject=cp->getIndex(this->getIndex());
        if(constantPoolObject->getConstantType()=="int"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="float"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="double"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="double"){
            javaFrame->getOperandStack()->pushInt(constantPoolObject->getIntValue());
            return;
        }
        if(constantPoolObject->getConstantType()=="string"){
            // javaFrame->getOperandStack().push(constantPoolObject->getIntValue());
            return;
        }
    }
}