//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_BISIPUSH_H
#define JAVAP_BISIPUSH_H


#include "../base/Instructions.h"

namespace Instruction{
    //将范围-128-127的整数型压栈
    class BiPush:public Instructions{
    private:
        u1 value;
    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    //将short类型数据压栈
    class SiPush:public Instructions{
    private:
        u2 value;
    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;
    };
}


#endif //JAVAP_BISIPUSH_H
