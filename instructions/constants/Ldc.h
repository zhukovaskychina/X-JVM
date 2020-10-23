//
// Created by zhukovasky on 2020/9/1.
//

#ifndef JVM_LDC_H
#define JVM_LDC_H


#include "../base/Instructions.h"

namespace Instruction{
    class LDC:public Instruction::Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class LDC_W:public Instruction::Index16Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class LDC2_W:public Instruction::Index16Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

}

#endif //JVM_LDC_H
