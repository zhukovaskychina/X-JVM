//
// Created by zhukovasky on 2020/8/31.
//

#include "GoTo.h"
namespace Instruction{
    void GoTo::execute(Runtime::JavaFrame *javaFrame) {
        int pc=javaFrame->getJavaThread()->getPcAddress();
        javaFrame->getJavaThread()->setPcAddress(pc+this->getOffset());
    }
}