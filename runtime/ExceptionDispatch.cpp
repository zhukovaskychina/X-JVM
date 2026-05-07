//
// 异常分派
//

#include "ExceptionDispatch.h"
#include "ClassLoader.h"
#include "JavaFrame.h"
#include "JavaThread.h"
#include "Object.h"
#include "TypeHelper.h"
#include "heap/ClassMember.h"
#include "heap/ConstantsPoolObject.h"
#include "heap/JavaClass.h"

namespace Runtime {

namespace {

using Heap::ConstantsPoolObject;
using Heap::Method;
using Heap::RuntimeConstantsPool;

bool tryHandlersInFrame(JavaFrame *frame, Object *ex, JavaClass *exCls, u2 pc) {
    Method *method = frame ? frame->getMethod() : nullptr;
    if (!frame || !method || !ex || !exCls) {
        return false;
    }
    for (const auto &h : method->getExceptionHandlers()) {
        if (pc < h.start_pc || pc >= h.end_pc) {
            continue;
        }
        if (h.catch_type != 0) {
            JavaClass *jc = method->getJavaClass();
            if (!jc) {
                continue;
            }
            RuntimeConstantsPool *cp = jc->getRuntimeConstantsPool();
            if (!cp) {
                continue;
            }
            const auto &vec = cp->getConstantsPoolObjectVector();
            if (h.catch_type < 1 || static_cast<size_t>(h.catch_type) > vec.size()) {
                continue;
            }
            ConstantsPoolObject *po = cp->getIndex(static_cast<int>(h.catch_type));
            if (!po) {
                continue;
            }
            Heap::ClassRef *cr = po->getClassRef();
            if (!cr) {
                continue;
            }
            JavaClass *catchCls = cr->resolvedJavaClass();
            ClassLoader *cl = jc->getClassLoader();
            if (!catchCls || !cl || !isJvmAssignable(exCls, catchCls, cl)) {
                continue;
            }
        }
        frame->getOperandStack()->clear();
        frame->getOperandStack()->pushRef(ex);
        frame->setNextPc(h.handler_pc);
        return true;
    }
    return false;
}

} // namespace

bool tryDispatchException(JavaThread *thread, Object *exceptionObject) {
    if (!thread || !exceptionObject || !exceptionObject->getJavaClass()) {
        return false;
    }
    JavaClass *exCls = exceptionObject->getJavaClass();
    JavaFrame *frame = thread->currentFrame();
    if (!frame) {
        return false;
    }
    u2 pc = static_cast<u2>(frame->getCurrentInsnBegin());
    for (;;) {
        if (tryHandlersInFrame(frame, exceptionObject, exCls, pc)) {
            return true;
        }
        const u2 siteInParent = static_cast<u2>(frame->getInvokeSitePc());
        thread->popJavaFrame();
        frame = thread->currentFrame();
        if (!frame) {
            return false;
        }
        pc = siteInParent;
    }
}

} // namespace Runtime
