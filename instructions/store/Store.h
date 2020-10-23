//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_STORE_H
#define JAVAP_STORE_H

//将变量从操作数栈顶弹出，然后存入局部变量表
#include "../base/Instructions.h"

namespace Instruction{

    class IStore:public Index8Instruction{
    private:

    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class IStore0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class IStore1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class IStore2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class IStore3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LStore:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LStore0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class LStore1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class LStore2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LStore3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class FStore:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class FStore0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class FStore1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class FStore2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class FStore3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class DStore:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class DStore0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class DStore1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class DStore2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class DStore3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class AStore:public Index8Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class AStore0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class AStore1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class AStore2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class AStore3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

}


#endif //JAVAP_STORE_H
