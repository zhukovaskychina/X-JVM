#ifndef XJVM_WIDE_H
#define XJVM_WIDE_H

#include "../base/Instructions.h"

namespace Instruction {

/** wide 前缀：支持 iload/lload/fload/dload/aload、istore/...、iinc。 */
class WideInst : public Instructions {
public:
    void fetchOperands(Instruction::ByteCodeReader* byteCodeReader) override;
    void execute(Runtime::JavaFrame* javaFrame) override;

private:
    u1 mod_{0};
    u2 idx_{0};
    u2 c_{0};
};

} // namespace Instruction

#endif
