//
// multianewarray（0xc5）
//

#ifndef JVM_MULTIANEWARRAY_H
#define JVM_MULTIANEWARRAY_H

#include "../base/Instructions.h"

namespace Instruction {

class MultiANewArray : public Instructions {
public:
    void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;
    void execute(Runtime::JavaFrame *javaFrame) override;

    u2 getPoolIndex() const { return poolIndex_; }
    u1 getDimCount() const { return dimCount_; }

private:
    u2 poolIndex_{0};
    u1 dimCount_{0};
};

} // namespace Instruction

#endif
