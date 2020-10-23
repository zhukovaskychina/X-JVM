//
// Created by zhukovasky on 2020/8/25.
//

#ifndef JVM_PUTSTATIC_H
#define JVM_PUTSTATIC_H


#include "../base/Instructions.h"

namespace Instruction{
    //指定类的静态域赋值

    class PutStatic :public Instruction::Index16Instruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

}


#endif //JVM_PUTSTATIC_H
