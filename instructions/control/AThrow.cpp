//
// athrow
//

#include "AThrow.h"
#include "../../runtime/ExceptionHelpers.h"
#include "../../runtime/JavaFrame.h"
#include "../../runtime/JvmThrownObject.h"
#include "../../runtime/Object.h"

namespace Instruction {

void AThrow::fetchOperands(ByteCodeReader *byteCodeReader) { (void)byteCodeReader; }

void AThrow::execute(Runtime::JavaFrame *javaFrame) {
    Runtime::Object *ref = javaFrame->getOperandStack()->popObject();
    if (!ref) {
        Runtime::throwNullPointerException(javaFrame);
        return;
    }
    throw Runtime::JvmThrownObject(ref);
}

} // namespace Instruction
