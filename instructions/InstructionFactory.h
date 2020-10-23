//
// Created by zhukovasky on 2020/8/29.
//

#ifndef JVM_INSTRUCTIONFACTORY_H
#define JVM_INSTRUCTIONFACTORY_H

#include "base/Instructions.h"

namespace Instruction{
    class InstructionFactory {


    public:
        static Instruction::Instructions* newInstruction(u1 opCode);

    };
}



#endif //JVM_INSTRUCTIONFACTORY_H
