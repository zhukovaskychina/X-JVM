//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_NOOP_H
#define JAVAP_NOOP_H

#include "../base/Instructions.h"

namespace Instruction{
    class NoOp: public NoOperationInstruction {
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

}



#endif //JAVAP_NOOP_H
