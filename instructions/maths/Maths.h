//
// Created by zhukovasky on 2020/8/14.
//

#ifndef JAVAP_MATHS_H
#define JAVAP_MATHS_H

#include "../base/Instructions.h"

namespace Instruction{
    class IRem:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LRem:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class FRem:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class DRem:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };


    class IAdd:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FAdd:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LAdd:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DAdd:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ISub:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FSub:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LSub:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DSub:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };


    class IDiv:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FDiv:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LDiv:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DDiv:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class IMul:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FMul:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LMul:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DMul:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class INeg:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FNeg:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LNeg:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DNeg:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IAnd:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class LAnd:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    ;;;;;
    class LXor:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IXor:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class Lor:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class Ior:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class ISHL:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ISHR:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IUSHR:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class LSHL:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class LSHR:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LUSHR:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class IInc:public Instructions{
    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;

    private:
        int index;
        int constants;
    };
}

#endif //JAVAP_MATHS_H
