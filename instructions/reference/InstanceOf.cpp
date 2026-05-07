//
// instanceof
//

#include "InstanceOf.h"

#include "../../runtime/JavaFrame.h"
#include "../../runtime/OperandStack.h"
#include "../../runtime/Object.h"
#include "../../runtime/TypeHelper.h"
#include "../../runtime/heap/ConstantsPoolObject.h"
#include "../../runtime/heap/JavaClass.h"

#include <stdexcept>

namespace Instruction {

void InstanceOf::execute(Runtime::JavaFrame *javaFrame) {
    Runtime::Object *ref = javaFrame->getOperandStack()->popObject();
    if (!ref) {
        javaFrame->getOperandStack()->pushInt(0);
        return;
    }
    Runtime::JavaClass *cur = javaFrame->getMethod()->getJavaClass();
    Runtime::Heap::RuntimeConstantsPool *cp = cur->getRuntimeConstantsPool();
    if (!cp) {
        throw std::runtime_error("instanceof: no constant pool");
    }
    Runtime::Heap::ConstantsPoolObject *po = cp->getIndex(this->getIndex());
    if (!po) {
        throw std::runtime_error("instanceof: bad pool index");
    }
    Runtime::Heap::ClassRef *cref = po->getClassRef();
    if (!cref) {
        throw std::runtime_error("instanceof: expected CONSTANT_Class");
    }
    Runtime::JavaClass *tgt = cref->resolvedJavaClass();
    Runtime::ClassLoader *cl = cur->getClassLoader();
    Runtime::JavaClass *valCls = ref->getJavaClass();
    const int ok =
        (tgt && cl && valCls && Runtime::isJvmAssignable(valCls, tgt, cl)) ? 1 : 0;
    javaFrame->getOperandStack()->pushInt(ok);
}

} // namespace Instruction
