//
// Created by zhukovasky on 2020/8/14.
//

#ifndef JAVAP_STACKCOMMAND_H
#define JAVAP_STACKCOMMAND_H


#include "../base/Instructions.h"

namespace Instruction{

    class Pop:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class Pop2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class Dup:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };


    class DupX1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class DupX2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class Dup2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class Dup2X1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class Dup2X2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class Swap:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
};


#endif //JAVAP_STACKCOMMAND_H
