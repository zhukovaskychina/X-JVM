//
// Created by zhukovasky on 2020/9/2.
//

#ifndef JVM_ARRAY_H
#define JVM_ARRAY_H


#include "../base/Instructions.h"

namespace Instruction{
    class NewArray:public Instruction::Instructions{
    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;


    private:
        u1 atype;
        Runtime::JavaClass* getPrimitiveArrayClass(Runtime::ClassLoader* classLoader,u1 atype);
    };

    /** anewarray：引用或数组成分的一维数组。 */
    class ANewArray : public Index16Instruction {
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    /** arraylength（0xbe） */
    class ArrayLength : public NoOperationInstruction {
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
}

#endif //JVM_ARRAY_H
