//
// Created by zhukovasky on 2020/8/31.
//

#ifndef JVM_INVOKESTATIC_H
#define JVM_INVOKESTATIC_H

#include "../base/Instructions.h"

namespace Instruction{
    class RuntimeConstantsPool;
    class InvokeStatic :public Instruction::Index16Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class InvokeVirtual:public Instruction::Index16Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class InvokeInterface:public Instruction::Instructions{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

    private:

    };

}


#endif //JVM_INVOKESTATIC_H
