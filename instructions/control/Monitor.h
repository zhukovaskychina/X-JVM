//
// monitorenter / monitorexit minimal monitor semantics.
//

#ifndef XJVM_MONITOR_H
#define XJVM_MONITOR_H

#include "../base/Instructions.h"

namespace Instruction {

class MonitorEnter : public NoOperationInstruction {
public:
    void execute(Runtime::JavaFrame* javaFrame) override;
};

class MonitorExit : public NoOperationInstruction {
public:
    void execute(Runtime::JavaFrame* javaFrame) override;
};

} // namespace Instruction

#endif
