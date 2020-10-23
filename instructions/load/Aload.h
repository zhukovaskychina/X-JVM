//
// Created by zhukovasky on 2020/9/8.
//

#ifndef JVM_ALOAD_H
#define JVM_ALOAD_H


#include "../base/Instructions.h"

namespace Instruction{
    class AAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class BAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class CAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LAload:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class SAload:public Instruction::NoOperationInstruction{

    };
}

#endif //JVM_ALOAD_H
