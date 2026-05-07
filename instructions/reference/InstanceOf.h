//
// instanceof（0xc1）
//

#ifndef JVM_INSTANCEOF_H
#define JVM_INSTANCEOF_H

#include "../base/Instructions.h"

namespace Instruction {

class InstanceOf : public Index16Instruction {
public:
    void execute(Runtime::JavaFrame *javaFrame) override;
};

} // namespace Instruction

#endif
