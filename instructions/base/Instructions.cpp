//
// Created by zhukovasky on 2020/8/13.
//

#include "Instructions.h"
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
        this->offset=byteCodeReader->readU2();
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
        javaThread->pushJavaFrame(newFrame);

        int argsCount=method->argsSlotCount();
        if (argsCount>0){
            for (int i = argsCount-1;i>=0; i--) {
                Runtime::Slots *slots=javaFrame->getOperandStack()->popSlots();
                newFrame->getLocalVariableTables()->setSlots(i,slots);
            }
        }
        if(method->isNative()){
            if(method->getName()=="registerNative"){
                javaThread->popJavaFrame();
            }
        }
    }
}