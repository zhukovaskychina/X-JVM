//
// Created by zhukovasky on 2020/8/31.
//

#ifndef JVM_COMPARISONS_H
#define JVM_COMPARISONS_H

#include "../base/Instructions.h"

namespace Instruction{

    class IfIcmpeq:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfIcmpne:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfIcmplt:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfIcmpge:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfIcmpgt:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfIcmple:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfAcmpeq:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IfAcmpne:public BranchInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
}


#endif //JVM_COMPARISONS_H
