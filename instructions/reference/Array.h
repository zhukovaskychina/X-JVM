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
}

#endif //JVM_ARRAY_H
