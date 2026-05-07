//
// getfield / putfield
//

#ifndef XJVM_FIELD_ACCESS_H
#define XJVM_FIELD_ACCESS_H

#include "../base/Instructions.h"

namespace Instruction {

class GetField : public Instruction::Index16Instruction {
public:
    void execute(Runtime::JavaFrame* javaFrame) override;
};

class PutField : public Instruction::Index16Instruction {
public:
    void execute(Runtime::JavaFrame* javaFrame) override;
};

} // namespace Instruction

#endif
