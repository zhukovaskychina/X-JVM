//
// 内嵌异常桩类安装
//

#include "SyntheticExceptionStubs.h"
#include "../ClassLoader.h"
#include "../heap/JavaClass.h"
#include "../../lang/ByteQueue.h"
#include <string>

namespace Runtime {

namespace {

#include "StubExceptionClassBytes.inc"

struct StubEntry {
    const char *binaryName;
    const unsigned char *bytes;
    unsigned int len;
};

static JavaClass *defineFromBytes(ClassLoader *cl, const unsigned char *data, unsigned int len) {
    auto *q = new Lang::ByteQueue();
    std::string s(reinterpret_cast<const char *>(data), static_cast<size_t>(len));
    q->pushStringChars(s);
    JavaClass *jc = cl->defineClass(q);
    if (jc) {
        cl->link(jc);
    }
    return jc;
}

} // namespace

bool ensureStubJavaLangExceptionChain(ClassLoader *cl) {
    if (!cl) {
        return false;
    }
    if (!cl->getLoadedClass("java/lang/Object")) {
        if (!cl->loadClass("java/lang/Object")) {
            return false;
        }
    }

    static const StubEntry kChain[] = {
        {"java/lang/Throwable", kStubThrowableClass, kStubThrowableClass_len},
        {"java/lang/Exception", kStubExceptionClass, kStubExceptionClass_len},
        {"java/lang/RuntimeException", kStubRuntimeExceptionClass, kStubRuntimeExceptionClass_len},
        {"java/lang/NullPointerException", kStubNullPointerExceptionClass, kStubNullPointerExceptionClass_len},
        {"java/lang/ReflectiveOperationException", kStubReflectiveOperationExceptionClass,
         kStubReflectiveOperationExceptionClass_len},
        {"java/lang/InstantiationException", kStubInstantiationExceptionClass, kStubInstantiationExceptionClass_len},
    };

    for (const StubEntry &e : kChain) {
        if (cl->getLoadedClass(e.binaryName)) {
            continue;
        }
        if (!defineFromBytes(cl, e.bytes, e.len)) {
            return false;
        }
    }

    return cl->getLoadedClass("java/lang/NullPointerException") != nullptr;
}

} // namespace Runtime
