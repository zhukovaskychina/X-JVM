//
// Created by zhukovasky on 2020/8/26.
//

#include "Interpret.h"
#include "../runtime/JavaThread.h"
#include "../instructions/base/ByteCodeReader.h"
#include "../instructions/InstructionFactory.h"

namespace Interpret{
    Interpret::Interpret() {}

    Interpret::~Interpret() {

    }

    void Interpret::execByteCode(Runtime::Heap::Method *method) {
        Runtime::JavaThread *javaThread=Runtime::JavaThread::newThread(1024);
        Runtime::JavaFrame *mainJavaFrame=new Runtime::JavaFrame(javaThread,method);
        javaThread->pushJavaFrame(mainJavaFrame);
        loopJavaFrame(javaThread);

    }

    void Interpret::loopJavaFrame(Runtime::JavaThread *javaThread) {

        Instruction::ByteCodeReader *byteCodeReader=new Instruction::ByteCodeReader();
        while (true){
            Runtime::JavaFrame* currentJavaFrame=javaThread->currentFrame();
            long nextPc=currentJavaFrame->getNextPc();
            javaThread->setPcAddress(nextPc);
            byteCodeReader->reset(currentJavaFrame->getMethod()->getCode(),nextPc);
            u1 opCode=byteCodeReader->readU1();
            Instruction::Instructions *instructions=Instruction::InstructionFactory::newInstruction(opCode);
            if(instructions== nullptr){
                cerr<<"指令加载异常"<<endl;
                exit(0);
            }
            instructions->fetchOperands(byteCodeReader);
            currentJavaFrame->setNextPc(byteCodeReader->getPc());
            instructions->execute(currentJavaFrame);
            if(javaThread->getJavaStack()->isEmpty()){
                break;
            }
            delete instructions;
        }
      //  delete byteCodeReader;
    }

    void Interpret::execByteCode(Runtime::Heap::Method *method, Runtime::JavaHeap *javaHeap) {

    }
}