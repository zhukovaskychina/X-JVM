//
// Created by zhukovasky on 2020/8/31.
//

#include "InvokeStatic.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../runtime/heap/ClassMember.h"
#include "../../runtime/heap/StringConstantPools.h"

namespace Instruction{
    void InvokeStatic::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
       // javaClass->getRuntimeConstantsPool();
        Runtime::Heap::RuntimeConstantsPool*  cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::MethodRef *methodRef=cp->getIndex(this->getIndex())->getMethodRef();
        Runtime::Heap::Method* method=methodRef->resolvedMethod();
        if(!method->isStatic()){

        }
       this->invokeMethod(javaFrame,method);
    }

    void InvokeVirtual::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        // javaClass->getRuntimeConstantsPool();
        Runtime::Heap::RuntimeConstantsPool*  cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::MethodRef *methodRef=cp->getIndex(this->getIndex())->getMethodRef();
        Runtime::Heap::Method* method=methodRef->resolvedMethod();

        if(method== nullptr){
            cerr<<"NoSuchMethodError"<<endl;
            exit(0);
        }

        if(!method->isStatic()){

        }

        Runtime::Object* objectRef=javaFrame->getOperandStack()->getRefFromTop(this->getIndex());
        if(objectRef== nullptr){
            cout<<"java.lang.NullPointerException"<<endl;

        }
        std::string methodName=methodRef->getName();

        if(methodName=="println"){
            Runtime::Object* object=javaFrame->getOperandStack()->popObject();
            std::string result=Runtime::Heap::JString::getJString()->getJavaString(object);
          //  std::string result=jString.getJavaString(object);
            cout<<result<<endl;
            return;
            //            exit(0);
        }
        //查找method
        Runtime::Heap::Method* methodsToBeInvoked=methodRef->lookupMethodInClass(objectRef->getJavaClass(),methodRef->getName(),methodRef->getMethod()->getDescriptor());

        this->invokeMethod(javaFrame,methodsToBeInvoked);
    }

    void InvokeInterface::execute(Runtime::JavaFrame *javaFrame) {

    }

    void InvokeInterface::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {

    }
}