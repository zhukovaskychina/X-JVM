//
// Created by zhukovasky on 2020/8/31.
//

#ifndef JVM_RETURN_H
#define JVM_RETURN_H

#include "../base/Instructions.h"

namespace Instruction{
    class ReturnInst :public Instruction::NoOperationInstruction{
    public:
        ReturnInst();

    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

        virtual ~ReturnInst();

    };
    class AReturn :public Instruction::NoOperationInstruction{


    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class DReturn :public Instruction::NoOperationInstruction{


    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class FReturn :public Instruction::NoOperationInstruction{


    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class IReturn :public Instruction::NoOperationInstruction{


    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class LReturn :public Instruction::NoOperationInstruction{


    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

}


#endif //JVM_RETURN_H
