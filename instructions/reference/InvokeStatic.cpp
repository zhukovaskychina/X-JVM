//
// Created by zhukovasky on 2020/8/31.
//

#include "InvokeStatic.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../runtime/heap/ClassMember.h"
#include "../../runtime/heap/StringConstantPools.h"
#include <iostream>
#include <stdexcept>

namespace Instruction{
    void InvokeStatic::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool*  cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::MethodRef *methodRef=cp->getIndex(this->getIndex())->getMethodRef();
        Runtime::Heap::Method* method=methodRef->resolvedMethod();
        if (method == nullptr) {
            throw std::runtime_error("InvokeStatic: unresolved method");
        }
        if (!method->isStatic()) {
            throw std::runtime_error("InvokeStatic: IncompatibleClassChangeError (non-static)");
        }
        this->invokeMethod(javaFrame, method);
    }

    void InvokeVirtual::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool*  cp=javaClass->getRuntimeConstantsPool();
        Runtime::Heap::MethodRef *methodRef=cp->getIndex(this->getIndex())->getMethodRef();
        Runtime::Heap::Method* resolved = methodRef->resolvedMethod();

        if(resolved == nullptr){
            throw std::runtime_error("InvokeVirtual: unresolved method (NoSuchMethodError)");
        }

        const int argc = resolved->argsSlotCount();
        Runtime::Object* objectRef = javaFrame->getOperandStack()->peekReceiverForInvoke(argc);
        if(objectRef== nullptr){
            throw std::runtime_error("InvokeVirtual: NullPointerException");
        }
        std::string methodName=methodRef->getName();

        if (methodName == "println" && resolved->getDescriptor().find("Ljava/lang/String;") != std::string::npos &&
            argc >= 2) {
            Runtime::Object* strObj = javaFrame->getOperandStack()->popObject();
            (void)javaFrame->getOperandStack()->popObject();
            std::string result = Runtime::Heap::JString::getJString()->getJavaString(strObj);
            std::cout << result << std::endl;
            return;
        }
        // 必须使用常量池符号上的 name/descriptor；resolvedMethod 与 getMethod() 在解析前可能不同步。
        Runtime::Heap::Method* methodsToBeInvoked = methodRef->lookupMethodInClass(
            objectRef->getJavaClass(), methodRef->getName(), methodRef->getDescriptor());

        if (methodsToBeInvoked == nullptr) {
            throw std::runtime_error("InvokeVirtual: method not found in class hierarchy");
        }

        this->invokeMethod(javaFrame,methodsToBeInvoked);
    }

    void InvokeSpecial::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass = javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool* cp = javaClass->getRuntimeConstantsPool();
        Runtime::Heap::ConstantsPoolObject* cpo = cp->getIndex(this->getIndex());
        if (!cpo || !cpo->getMethodRef()) {
            throw std::runtime_error("InvokeSpecial: invalid constant pool entry");
        }
        Runtime::Heap::MethodRef* methodRef = cpo->getMethodRef();
        Runtime::Heap::Method* resolved = methodRef->resolvedMethod();
        if (resolved == nullptr) {
            throw std::runtime_error("InvokeSpecial: unresolved method");
        }
        const int argc = resolved->argsSlotCount();
        Runtime::Object* recv = javaFrame->getOperandStack()->peekReceiverForInvoke(argc);
        if (recv == nullptr) {
            throw std::runtime_error("InvokeSpecial: NullPointerException");
        }
        this->invokeMethod(javaFrame, resolved);
    }

    void InvokeInterface::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::JavaClass* javaClass = javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool* cp = javaClass->getRuntimeConstantsPool();
        Runtime::Heap::ConstantsPoolObject* cpo = cp->getIndex(this->getIndex());
        Runtime::Heap::InterfaceMethodRef* imr = cpo ? cpo->getInterfaceMethodRef() : nullptr;
        if (!imr) {
            throw std::runtime_error("InvokeInterface: not an InterfaceMethodRef");
        }
        Runtime::Heap::Method* ifaceMethod = imr->resolvedInterfaceMethod();
        if (!ifaceMethod) {
            throw std::runtime_error("InvokeInterface: unresolved interface method");
        }
        const int argc = ifaceMethod->argsSlotCount();
        Runtime::Object* recv = javaFrame->getOperandStack()->peekReceiverForInvoke(argc);
        if (recv == nullptr) {
            throw std::runtime_error("InvokeInterface: NullPointerException");
        }
        Runtime::Heap::MethodRef helper;
        Runtime::Heap::Method* toCall = helper.lookupMethodInClass(
            recv->getJavaClass(), imr->getName(), imr->getDescriptor());
        if (!toCall) {
            throw std::runtime_error("InvokeInterface: no implementation in receiver class");
        }
        // count：objectref 与各参数所占栈槽之和，须与 argsSlotCount 一致（JVMS invokeinterface）。
        if (static_cast<int>(count_) != argc) {
            throw std::runtime_error(
                "InvokeInterface: count operand does not match method argsSlotCount");
        }
        this->invokeMethod(javaFrame, toCall);
    }

    void InvokeInterface::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        Index16Instruction::fetchOperands(byteCodeReader);
        count_ = byteCodeReader->readU1();
        (void)byteCodeReader->readU1();
    }
}
