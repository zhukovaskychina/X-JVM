//
// 异常辅助
//

#include "ExceptionHelpers.h"
#include "ClassLoader.h"
#include "JavaFrame.h"
#include "JvmThrownObject.h"
#include "Object.h"
#include "heap/ClassMember.h"
#include "heap/JavaClass.h"
#include "synthetic/SyntheticExceptionStubs.h"
#include <stdexcept>

namespace Runtime {

Object *newNullPointerExceptionInstance(JavaFrame *frame) {
    if (!frame || !frame->getMethod()) {
        return nullptr;
    }
    Heap::Method *m = frame->getMethod();
    JavaClass *cur = m->getJavaClass();
    if (!cur) {
        return nullptr;
    }
    ClassLoader *cl = cur->getClassLoader();
    if (!cl) {
        return nullptr;
    }
    JavaClass *npeClass = cl->loadClass("java/lang/NullPointerException");
    if (!npeClass) {
        if (!ensureStubJavaLangExceptionChain(cl)) {
            return nullptr;
        }
        npeClass = cl->loadClass("java/lang/NullPointerException");
    }
    if (!npeClass) {
        return nullptr;
    }
    return npeClass->createNewJavaObjectInstance();
}

void throwNullPointerException(JavaFrame *frame) {
    Object *ex = newNullPointerExceptionInstance(frame);
    if (ex) {
        throw JvmThrownObject(ex);
    }
    throw std::runtime_error("java.lang.NullPointerException");
}

Object *newInstantiationExceptionInstance(JavaFrame *frame) {
    if (!frame || !frame->getMethod()) {
        return nullptr;
    }
    Heap::Method *m = frame->getMethod();
    JavaClass *cur = m->getJavaClass();
    if (!cur) {
        return nullptr;
    }
    ClassLoader *cl = cur->getClassLoader();
    if (!cl) {
        return nullptr;
    }
    JavaClass *ieClass = cl->loadClass("java/lang/InstantiationException");
    if (!ieClass) {
        if (!ensureStubJavaLangExceptionChain(cl)) {
            return nullptr;
        }
        ieClass = cl->loadClass("java/lang/InstantiationException");
    }
    if (!ieClass) {
        return nullptr;
    }
    return ieClass->createNewJavaObjectInstance();
}

void throwInstantiationException(JavaFrame *frame) {
    Object *ex = newInstantiationExceptionInstance(frame);
    if (ex) {
        throw JvmThrownObject(ex);
    }
    throw std::runtime_error("java.lang.InstantiationException");
}

} // namespace Runtime
