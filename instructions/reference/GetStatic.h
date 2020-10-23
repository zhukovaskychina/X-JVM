//
// Created by zhukovasky on 2020/8/25.
//

#ifndef JVM_GETSTATIC_H
#define JVM_GETSTATIC_H


#include "../base/Instructions.h"

namespace Instruction{
    class GetStatic: public Index16Instruction {
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
}


#endif //JVM_GETSTATIC_H
