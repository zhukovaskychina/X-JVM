//
// Created by zhukovasky on 2020/8/25.
//

#ifndef JVM_NEWREF_H
#define JVM_NEWREF_H

#include "../base/Instructions.h"
namespace Instruction{
    class NewRef: public Index16Instruction {
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
}



#endif //JVM_NEWREF_H
