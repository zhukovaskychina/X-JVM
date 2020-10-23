//
// Created by zhukovasky on 2020/9/8.
//

#ifndef JVM_ASTORE_H
#define JVM_ASTORE_H

#include "../base/Instructions.h"

namespace Instruction{
    class AAStore:public Instruction::NoOperationInstruction{
    public:


    };
    class BAStore:public Instruction::NoOperationInstruction{
    public:


    };
    class CAStore:public Instruction::NoOperationInstruction{
    public:


    };
    class DAStore:public Instruction::NoOperationInstruction{
    public:


    };
    class FAStore:public Instruction::NoOperationInstruction{
    public:


    };
    class IAStore:public Instruction::NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LAStore:public Instruction::NoOperationInstruction{
    public:


    };
    class SAStore:public Instruction::NoOperationInstruction{
    public:


    };


}


#endif //JVM_ASTORE_H
