//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_LOAD_H
#define JAVAP_LOAD_H


#include "../base/Instructions.h"

namespace Instruction{

    class ILoad:public Index8Instruction{
    private:

    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class ILoad0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ILoad1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class ILoad2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class ILoad3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LLoad:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LLoad0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class LLoad1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class LLoad2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LLoad3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class FLoad:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class FLoad0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class FLoad1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class FLoad2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class FLoad3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class DLoad:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class DLoad0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class DLoad1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class DLoad2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class DLoad3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class ALoad:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class ALoad0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class ALoad1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class ALoad2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class ALoad3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };


}

#endif //JAVAP_LOAD_H
