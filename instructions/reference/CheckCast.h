//
// checkcast（0xc0）
//

#ifndef JVM_CHECKCAST_H
#define JVM_CHECKCAST_H

#include "../base/Instructions.h"

namespace Instruction {

class CheckCast : public Index16Instruction {
public:
    void execute(Runtime::JavaFrame *javaFrame) override;
};

} // namespace Instruction

#endif
