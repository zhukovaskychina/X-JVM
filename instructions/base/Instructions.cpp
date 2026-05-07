//
// Created by zhukovasky on 2020/8/13.
//

#include "Instructions.h"
#include "../../runtime/native/NativeMethodRegistry.h"
#include "../../runtime/heap/JavaClass.h"
#include <iostream>

namespace Instruction{
    Instructions::Instructions() = default;

    Instructions::~Instructions() = default;

    NoOperationInstruction::NoOperationInstruction() {
       // cout<<"初始化"<<endl;
    }

    NoOperationInstruction::~NoOperationInstruction() {
       // cout<<"销毁"<<endl;
    }

    BranchInstruction::BranchInstruction() {}

    BranchInstruction::~BranchInstruction() {

    }
    Index16Instruction::Index16Instruction() {}

    Index16Instruction::~Index16Instruction() {

    }

    Index8Instruction::Index8Instruction() {}

    Index8Instruction::~Index8Instruction() {

    }

    u1 Index8Instruction::getIndex() const {
        return index;
    }

    void Index8Instruction::setIndex(u1 index) {
        Index8Instruction::index = index;
    }

    //
    void NoOperationInstruction::fetchOperands(Instruction::ByteCodeReader *byteCodeReader)  {
        //Instructions::fetchOperands(byteCodeReader);
    }

    void NoOperationInstruction::execute(Runtime::JavaFrame *javaFrame) {

    }

    u2 Index16Instruction::getIndex() const {
        return index;
    }

    void Index16Instruction::setIndex(u2 index) {
        Index16Instruction::index = index;
    }

    //用于跳转指令
    //offset用于存放跳转偏移量
    void BranchInstruction::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        const u1 hi = byteCodeReader->readU1();
        const u1 lo = byteCodeReader->readU1();
        const u2 raw = static_cast<u2>((static_cast<u2>(hi) << 8) | static_cast<u2>(lo));
        this->offset = static_cast<int16_t>(raw);
    }

    int BranchInstruction::getOffset() const {
        return offset;
    }


    void Index8Instruction::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        this->index=byteCodeReader->readU1();
    }

    void Index16Instruction::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        this->index=byteCodeReader->readU2();
    }

    void Index16Instruction::invokeMethod(Runtime::JavaFrame *javaFrame, Runtime::Heap::Method *method) {
        Runtime::JavaThread* javaThread=javaFrame->getJavaThread();
        Runtime::JavaFrame* newFrame=new Runtime::JavaFrame(javaThread,method);
        newFrame->setInvokeSitePc(javaFrame->getCurrentInsnBegin());
        javaThread->pushJavaFrame(newFrame);

        int argsCount=method->argsSlotCount();
        if (argsCount>0){
            for (int i = argsCount-1;i>=0; i--) {
                Runtime::Slots *slots=javaFrame->getOperandStack()->popSlots();
                newFrame->getLocalVariableTables()->setSlots(i,slots);
            }
        }
        if (method->isNative()) {
            Runtime::Native::NativeMethodRegistry& natives = Runtime::Native::NativeMethodRegistry::instance();
            try {
                if (natives.tryInvoke(javaFrame, newFrame, method)) {
                    javaThread->popJavaFrame();
                } else {
                    Runtime::JavaClass* jc = method->getJavaClass();
                    std::cerr << "[X-JVM] Unregistered native method: "
                              << (jc ? jc->getThisClassName() : std::string("?")) << "."
                              << method->getName() << " " << method->getDescriptor() << std::endl;
                    javaThread->popJavaFrame();
                }
            } catch (...) {
                javaThread->popJavaFrame();
                throw;
            }
        }
    }
}