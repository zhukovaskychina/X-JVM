//
// Created by zhukovasky on 2020/8/31.
//

#ifndef JVM_GOTO_H
#define JVM_GOTO_H

#include "../base/Instructions.h"

namespace Instruction{
    class GoTo: public Instruction::BranchInstruction {
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
}



#endif //JVM_GOTO_H
