//
// athrow (0xbf)
//

#ifndef XJVM_INSTRUCTION_ATHROW_H
#define XJVM_INSTRUCTION_ATHROW_H

#include "../base/Instructions.h"

namespace Instruction {

class AThrow : public NoOperationInstruction {
public:
    void fetchOperands(ByteCodeReader *byteCodeReader) override;

    void execute(Runtime::JavaFrame *javaFrame) override;
};

} // namespace Instruction

#endif
