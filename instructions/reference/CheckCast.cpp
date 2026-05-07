//
// checkcast
//

#include "CheckCast.h"

#include "../../runtime/JavaFrame.h"
#include "../../runtime/OperandStack.h"
#include "../../runtime/Object.h"
#include "../../runtime/TypeHelper.h"
#include "../../runtime/heap/ConstantsPoolObject.h"
#include "../../runtime/heap/JavaClass.h"

#include <stdexcept>

namespace Instruction {

void CheckCast::execute(Runtime::JavaFrame *javaFrame) {
    Runtime::Object *ref = javaFrame->getOperandStack()->popObject();
    if (!ref) {
        javaFrame->getOperandStack()->pushRef(nullptr);
        return;
    }
    Runtime::JavaClass *cur = javaFrame->getMethod()->getJavaClass();
    Runtime::Heap::RuntimeConstantsPool *cp = cur->getRuntimeConstantsPool();
    if (!cp) {
        throw std::runtime_error("checkcast: no constant pool");
    }
    Runtime::Heap::ConstantsPoolObject *po = cp->getIndex(this->getIndex());
    if (!po) {
        throw std::runtime_error("checkcast: bad pool index");
    }
    Runtime::Heap::ClassRef *cref = po->getClassRef();
    if (!cref) {
        throw std::runtime_error("checkcast: expected CONSTANT_Class");
    }
    Runtime::JavaClass *tgt = cref->resolvedJavaClass();
    Runtime::ClassLoader *cl = cur->getClassLoader();
    Runtime::JavaClass *valCls = ref->getJavaClass();
    if (!tgt || !cl || !valCls || !Runtime::isJvmAssignable(valCls, tgt, cl)) {
        throw std::runtime_error("ClassCastException");
    }
    javaFrame->getOperandStack()->pushRef(ref);
}

} // namespace Instruction
