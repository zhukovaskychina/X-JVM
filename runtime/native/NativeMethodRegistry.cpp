//
// Native 方法注册表与 JDK 常用桩实现
//

#include "NativeMethodRegistry.h"
#include "FileNativeSupport.h"
#include "NetworkNativeSupport.h"
#include "NioChNativeSupport.h"
#include "UnsafeNativeSupport.h"

#include "../ClassLoader.h"
#include "../JavaFrame.h"
#include "../JavaThread.h"
#include "../LocalVariableTables.h"
#include "../Object.h"
#include "../OperandStack.h"
#include "../heap/ClassMember.h"
#include "../heap/JavaClass.h"
#include "../heap/StringConstantPools.h"
#include "../threading/ObjectMonitor.h"

#include "../../utils/StringUtils.h"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Runtime {
namespace Native {

std::mutex g_auxMutex;
std::unordered_map<JavaClass*, Object*> g_classMirrors;
Object* g_mainThreadStub = nullptr;

static void clearAuxStateLocked() {
    unsafeParkBindCurrentThreadOop(nullptr);
    for (auto& kv : g_classMirrors) {
        delete kv.second;
    }
    g_classMirrors.clear();
    delete g_mainThreadStub;
    g_mainThreadStub = nullptr;
}

JavaClass* javaClassFromClassMirrorObject(Object* mirror) {
    if (!mirror) {
        return nullptr;
    }
    return static_cast<JavaClass*>(mirror->getData());
}

Object* mirrorForJavaClass(JavaClass* jc, ClassLoader* cl) {
    if (!jc || !cl) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_auxMutex);
    auto it = g_classMirrors.find(jc);
    if (it != g_classMirrors.end()) {
        return it->second;
    }
    JavaClass* meta = cl->loadClass("java/lang/Class");
    if (!meta) {
        return nullptr;
    }
    auto* m = new Object();
    m->setJavaClass(meta);
    m->setData(reinterpret_cast<void*>(jc));
    g_classMirrors[jc] = m;
    return m;
}

void ensureParkTlsBoundForLoader(ClassLoader* cl) {
    if (!cl) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_auxMutex);
    if (!g_mainThreadStub) {
        JavaClass* thCls = cl->loadClass("java/lang/Thread");
        if (!thCls) {
            return;
        }
        g_mainThreadStub = new Object();
        g_mainThreadStub->setJavaClass(thCls);
    }
    unsafeParkBindCurrentThreadOop(g_mainThreadStub);
}

namespace {

ClassLoader* loaderFromFrame(JavaFrame* f) {
    if (!f || !f->getMethod() || !f->getMethod()->getJavaClass()) {
        return nullptr;
    }
    return f->getMethod()->getJavaClass()->getClassLoader();
}

JavaClass* targetFromClassMirror(Object* mirror) {
    return javaClassFromClassMirrorObject(mirror);
}

Object* mirrorFor(JavaClass* jc, ClassLoader* cl) {
    return mirrorForJavaClass(jc, cl);
}

bool isSubtype(JavaClass* sub, JavaClass* sup) {
    if (!sub || !sup) {
        return false;
    }
    if (sub == sup) {
        return true;
    }
    for (JavaClass* p = sub->getSuperClass(); p; p = p->getSuperClass()) {
        if (p == sup) {
            return true;
        }
    }
    return false;
}

bool nameLooksPrimitive(JavaClass* jc) {
    if (!jc) {
        return false;
    }
    const std::string& n = jc->getThisClassName();
    if (n.size() == 1) {
        return std::string("IJZBSFCD").find(n[0]) != std::string::npos;
    }
    return false;
}

std::string dottedInternalName(JavaClass* jc) {
    if (!jc) {
        return "";
    }
    std::string n = jc->getThisClassName();
    for (char& c : n) {
        if (c == '/') {
            c = '.';
        }
    }
    return n;
}

/** Class.forName 二进制名 -> 内部名（如 java.lang.String -> java/lang/String，[Ljava.lang.Object; -> [Ljava/lang/Object;）。 */
std::string binaryNameToInternal(const std::string& binary) {
    if (binary.empty()) {
        return binary;
    }
    if (binary[0] == '[') {
        size_t i = 0;
        while (i < binary.size() && binary[i] == '[') {
            ++i;
        }
        if (i < binary.size() && binary[i] == 'L') {
            const size_t semi = binary.find(';', i);
            if (semi == std::string::npos) {
                return binary;
            }
            std::string out = binary.substr(0, i + 1);
            for (size_t j = i + 1; j < semi; ++j) {
                char c = binary[j];
                out.push_back(c == '.' ? '/' : c);
            }
            out.append(binary.substr(semi));
            return out;
        }
        return binary;
    }
    std::string out = binary;
    for (char& c : out) {
        if (c == '.') {
            c = '/';
        }
    }
    return out;
}

size_t arrayDescriptorElemBytes(const std::string& n) {
    if (n.size() < 2 || n[0] != '[') {
        return 0;
    }
    const char t = n[1];
    if (t == 'L' || t == '[') {
        return 0;
    }
    switch (t) {
    case 'Z':
    case 'B':
        return 1;
    case 'S':
    case 'C':
        return 2;
    case 'I':
    case 'F':
        return 4;
    case 'J':
    case 'D':
        return 8;
    default:
        return 0;
    }
}

void native_void_noop(const NativeCallContext&) {}

void native_System_registerNatives(const NativeCallContext&) {}

void native_System_setIn0(const NativeCallContext&) {}

void native_System_setOut0(const NativeCallContext&) {}

void native_System_setErr0(const NativeCallContext&) {}

void native_System_nanoTime(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::steady_clock::now().time_since_epoch())
                        .count();
    ctx.caller->getOperandStack()->pushLong(static_cast<long>(ns));
}

void native_System_currentTimeMillis(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();
    ctx.caller->getOperandStack()->pushLong(static_cast<long>(ms));
}

void native_System_arraycopy(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* src = lvt->getRef(0);
    const int srcPos = lvt->getInt(1);
    Object* dest = lvt->getRef(2);
    const int destPos = lvt->getInt(3);
    const int len = lvt->getInt(4);
    if (len == 0) {
        return;
    }
    if (!src || !dest) {
        return;
    }
    JavaClass* srcCl = src->getJavaClass();
    JavaClass* destCl = dest->getJavaClass();
    if (!srcCl || !destCl) {
        return;
    }
    const std::string& srcName = srcCl->getThisClassName();
    if (srcName != destCl->getThisClassName()) {
        return;
    }
    size_t elem = arrayDescriptorElemBytes(srcName);
    if (elem == 0) {
        if (!srcName.empty() && srcName[0] == '[') {
            elem = sizeof(Object*);
        }
    }
    if (elem == 0) {
        return;
    }
    const int srcLen = src->getArrayLength();
    const int destLen = dest->getArrayLength();
    if (srcPos < 0 || destPos < 0 || len < 0) {
        return;
    }
    const auto srcEnd = static_cast<std::int64_t>(srcPos) + static_cast<std::int64_t>(len);
    const auto destEnd = static_cast<std::int64_t>(destPos) + static_cast<std::int64_t>(len);
    if (srcEnd > srcLen || destEnd > destLen) {
        return;
    }
    void* sp = src->getData();
    void* dp = dest->getData();
    if (!sp || !dp) {
        return;
    }
    auto* sb = static_cast<std::uint8_t*>(sp);
    auto* db = static_cast<std::uint8_t*>(dp);
    std::memmove(db + static_cast<size_t>(destPos) * elem, sb + static_cast<size_t>(srcPos) * elem,
                 static_cast<size_t>(len) * elem);
}

void native_System_identityHashCode(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* o = ctx.callee->getLocalVariableTables()->getRef(0);
    std::uintptr_t bits = reinterpret_cast<std::uintptr_t>(o);
    std::uint32_t h = static_cast<std::uint32_t>(bits ^ (bits >> 32));
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(h));
}

void native_System_mapLibraryName(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* in = ctx.callee->getLocalVariableTables()->getRef(0);
    ctx.caller->getOperandStack()->pushRef(in);
}

void native_Object_hashCode(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    std::uintptr_t bits = reinterpret_cast<std::uintptr_t>(self);
    std::uint32_t h = static_cast<std::uint32_t>(bits ^ (bits >> 32));
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(h));
}

void native_Object_getClass(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    if (!self) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    JavaClass* jc = self->getJavaClass();
    ClassLoader* cl = loaderFromFrame(ctx.callee);
    if (!jc || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(mirrorFor(jc, cl));
}

void native_Object_clone(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    // 基础桩：返回同一引用（非规范，可避免未实现拷贝时崩溃）
    ctx.caller->getOperandStack()->pushRef(self);
}

void native_Object_notify(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    JavaThread* th = ctx.callee->getJavaThread();
    Runtime::Threading::ObjectMonitorTable::instance().monitorNotify(self, th);
}

void native_Object_notifyAll(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    JavaThread* th = ctx.callee->getJavaThread();
    Runtime::Threading::ObjectMonitorTable::instance().monitorNotifyAll(self, th);
}

void native_Object_wait0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    const long millis = ctx.callee->getLocalVariableTables()->getLong(1);
    JavaThread* th = ctx.callee->getJavaThread();
    Runtime::Threading::ObjectMonitorTable::instance().monitorWait(self, th, millis);
}

void native_String_intern(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    ctx.caller->getOperandStack()->pushRef(self);
}

void native_Class_registerNatives(const NativeCallContext&) {}

void native_Class_forName0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* nameObj = ctx.callee->getLocalVariableTables()->getRef(0);
    // int initFlag = ctx.callee->getLocalVariableTables()->getInt(1); // 初始化需解释器配合 clinit 帧，暂不处理
    Object* loaderObj = ctx.callee->getLocalVariableTables()->getRef(2);
    (void)loaderObj;
    ClassLoader* cl = loaderFromFrame(ctx.callee);
    if (!nameObj || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    const std::string binary = Heap::JString::getJString()->getJavaString(nameObj);
    const std::string internal = binaryNameToInternal(binary);
    if (internal.empty()) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    JavaClass* jc = cl->loadClass(internal);
    if (!jc) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(mirrorFor(jc, cl));
}

void native_Class_isInstance(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(0);
    Object* obj = ctx.callee->getLocalVariableTables()->getRef(1);
    JavaClass* target = targetFromClassMirror(mirror);
    JavaClass* ot = obj ? obj->getJavaClass() : nullptr;
    const int v = (target && ot && isSubtype(ot, target)) ? 1 : 0;
    ctx.caller->getOperandStack()->pushInt(v);
}

void native_Class_isAssignableFrom(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* a = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    JavaClass* b = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(1));
    const int v = isSubtype(b, a) ? 1 : 0;
    ctx.caller->getOperandStack()->pushInt(v);
}

void native_Class_isInterface(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    const int v = (t && t->isInterface()) ? 1 : 0;
    ctx.caller->getOperandStack()->pushInt(v);
}

void native_Class_isArray(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    const int v = (t && t->isArray()) ? 1 : 0;
    ctx.caller->getOperandStack()->pushInt(v);
}

void native_Class_isPrimitive(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    const int v = nameLooksPrimitive(t) ? 1 : 0;
    ctx.caller->getOperandStack()->pushInt(v);
}

void native_Class_initClassName(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    ClassLoader* cl = loaderFromFrame(ctx.callee);
    if (!t || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    std::string name = dottedInternalName(t);
    JavaHeap* heap = ctx.callee->getJavaThread() ? ctx.callee->getJavaThread()->getJavaHeap() : nullptr;
    Object* str = Heap::JString::getJString()->getJString(cl, name, heap);
    ctx.caller->getOperandStack()->pushRef(str);
}

void native_Class_getSuperclass(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    ClassLoader* cl = loaderFromFrame(ctx.callee);
    if (!t || !cl || !t->getSuperClass()) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(mirrorFor(t->getSuperClass(), cl));
}

void native_Class_getModifiers(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    const int m = t ? static_cast<int>(t->getClassAccessFlags() & 0xFFFF) : 0;
    ctx.caller->getOperandStack()->pushInt(m);
}

void native_Class_getClassAccessFlagsRaw0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaClass* t = targetFromClassMirror(ctx.callee->getLocalVariableTables()->getRef(0));
    const int m = t ? static_cast<int>(t->getClassAccessFlags() & 0xFFFF) : 0;
    ctx.caller->getOperandStack()->pushInt(m);
}

void native_Class_desiredAssertionStatus0(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_Class_isRecord0(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_Class_isHidden(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_Class_getClassFileVersion0(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ctx.caller->getOperandStack()->pushInt(52);
}

void native_ref_null(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
}

void native_Class_setSigners(const NativeCallContext&) {}

void native_Thread_registerNatives(const NativeCallContext&) {}

void native_Thread_currentThread(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ClassLoader* cl = loaderFromFrame(ctx.caller);
    if (!cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ensureParkTlsBoundForLoader(cl);
    std::lock_guard<std::mutex> lock(g_auxMutex);
    ctx.caller->getOperandStack()->pushRef(g_mainThreadStub);
}

void native_Thread_currentCarrierThread(const NativeCallContext& ctx) {
    native_Thread_currentThread(ctx);
}

void native_Thread_holdsLock(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* target = ctx.callee->getLocalVariableTables()->getRef(0);
    JavaThread* th = ctx.callee->getJavaThread();
    const bool held = Runtime::Threading::ObjectMonitorTable::instance().holdsLock(target, th);
    ctx.caller->getOperandStack()->pushInt(held ? 1 : 0);
}

void native_Thread_interrupt0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    JavaThread* th = ctx.callee->getJavaThread();
    if (th) {
        th->interrupt();
    }
}

void native_Thread_isInterrupted(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaThread* th = ctx.callee->getJavaThread();
    const int clear = ctx.callee->getLocalVariableTables()->getInt(1);
    const bool interrupted = th ? th->isInterrupted(clear != 0) : false;
    ctx.caller->getOperandStack()->pushInt(interrupted ? 1 : 0);
}

void native_Thread_interrupted(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    JavaThread* th = ctx.callee->getJavaThread();
    const bool interrupted = th ? th->isInterrupted(true) : false;
    ctx.caller->getOperandStack()->pushInt(interrupted ? 1 : 0);
}

void native_Thread_clearInterruptEvent(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    JavaThread* th = ctx.callee->getJavaThread();
    if (th) {
        (void) th->isInterrupted(true);
    }
}

void native_Thread_getNextThreadIdOffset(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ctx.caller->getOperandStack()->pushLong(0);
}

void native_Class_getPrimitiveClass(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* strObj = ctx.callee->getLocalVariableTables()->getRef(0);
    ClassLoader* cl = loaderFromFrame(ctx.callee);
    if (!strObj || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    const std::string name = Heap::JString::getJString()->getJavaString(strObj);
    static const std::unordered_map<std::string, const char*> prim = {
        {"boolean", "Z"}, {"byte", "B"},   {"char", "C"},    {"short", "S"},
        {"int", "I"},     {"long", "J"},   {"float", "F"},   {"double", "D"},
        {"void", "V"},
    };
    auto it = prim.find(name);
    if (it == prim.end()) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    JavaClass* jc = cl->loadClass(std::string(1, it->second[0]));
    if (!jc) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(mirrorFor(jc, cl));
}

} // namespace

std::string NativeMethodRegistry::makeKey(const std::string& cls, const std::string& name,
                                          const std::string& desc) {
    std::string k;
    k.reserve(cls.size() + name.size() + desc.size() + 2);
    k.append(cls);
    k.push_back('\x1E');
    k.append(name);
    k.push_back('\x1E');
    k.append(desc);
    return k;
}

NativeMethodRegistry::NativeMethodRegistry() {
    registerBuiltInsUnlocked();
}

NativeMethodRegistry& NativeMethodRegistry::instance() {
    static NativeMethodRegistry inst;
    return inst;
}

void NativeMethodRegistry::registerNativeUnlocked(const std::string& classInternalName,
                                                  const std::string& methodName,
                                                  const std::string& descriptor,
                                                  NativeMethodHandler handler) {
    handlers_[makeKey(classInternalName, methodName, descriptor)] = handler;
}

void NativeMethodRegistry::registerBuiltInsUnlocked() {
    // java.lang.System
    registerNativeUnlocked("java/lang/System", "registerNatives", "()V", native_System_registerNatives);
    registerNativeUnlocked("java/lang/System", "setIn0", "(Ljava/io/InputStream;)V", native_System_setIn0);
    registerNativeUnlocked("java/lang/System", "setOut0", "(Ljava/io/PrintStream;)V", native_System_setOut0);
    registerNativeUnlocked("java/lang/System", "setErr0", "(Ljava/io/PrintStream;)V", native_System_setErr0);
    registerNativeUnlocked("java/lang/System", "nanoTime", "()J", native_System_nanoTime);
    registerNativeUnlocked("java/lang/System", "currentTimeMillis", "()J", native_System_currentTimeMillis);
    registerNativeUnlocked("java/lang/System", "arraycopy",
                           "(Ljava/lang/Object;ILjava/lang/Object;II)V", native_System_arraycopy);
    registerNativeUnlocked("java/lang/System", "identityHashCode", "(Ljava/lang/Object;)I",
                           native_System_identityHashCode);
    registerNativeUnlocked("java/lang/System", "mapLibraryName", "(Ljava/lang/String;)Ljava/lang/String;",
                           native_System_mapLibraryName);

    // java.lang.Object
    registerNativeUnlocked("java/lang/Object", "hashCode", "()I", native_Object_hashCode);
    registerNativeUnlocked("java/lang/Object", "getClass", "()Ljava/lang/Class;", native_Object_getClass);
    registerNativeUnlocked("java/lang/Object", "clone", "()Ljava/lang/Object;", native_Object_clone);
    registerNativeUnlocked("java/lang/Object", "notify", "()V", native_Object_notify);
    registerNativeUnlocked("java/lang/Object", "notifyAll", "()V", native_Object_notifyAll);
    registerNativeUnlocked("java/lang/Object", "wait0", "(J)V", native_Object_wait0);

    // java.lang.String
    registerNativeUnlocked("java/lang/String", "intern", "()Ljava/lang/String;", native_String_intern);

    // java.lang.Class
    registerNativeUnlocked("java/lang/Class", "registerNatives", "()V", native_Class_registerNatives);
    registerNativeUnlocked("java/lang/Class", "forName0",
                           "(Ljava/lang/String;ZLjava/lang/ClassLoader;Ljava/lang/Class;)Ljava/lang/Class;",
                           native_Class_forName0);
    registerNativeUnlocked("java/lang/Class", "isInstance", "(Ljava/lang/Object;)Z", native_Class_isInstance);
    registerNativeUnlocked("java/lang/Class", "isAssignableFrom", "(Ljava/lang/Class;)Z",
                           native_Class_isAssignableFrom);
    registerNativeUnlocked("java/lang/Class", "isInterface", "()Z", native_Class_isInterface);
    registerNativeUnlocked("java/lang/Class", "isArray", "()Z", native_Class_isArray);
    registerNativeUnlocked("java/lang/Class", "isPrimitive", "()Z", native_Class_isPrimitive);
    registerNativeUnlocked("java/lang/Class", "initClassName", "()Ljava/lang/String;", native_Class_initClassName);
    registerNativeUnlocked("java/lang/Class", "getSuperclass", "()Ljava/lang/Class;", native_Class_getSuperclass);
    registerNativeUnlocked("java/lang/Class", "getInterfaces0", "()[Ljava/lang/Class;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getModifiers", "()I", native_Class_getModifiers);
    registerNativeUnlocked("java/lang/Class", "getSigners", "()[Ljava/lang/Object;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "setSigners", "([Ljava/lang/Object;)V", native_Class_setSigners);
    registerNativeUnlocked("java/lang/Class", "getEnclosingMethod0", "()[Ljava/lang/Object;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getDeclaringClass0", "()Ljava/lang/Class;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getSimpleBinaryName0", "()Ljava/lang/String;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getProtectionDomain0", "()Ljava/security/ProtectionDomain;",
                           native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getPrimitiveClass", "(Ljava/lang/String;)Ljava/lang/Class;",
                           native_Class_getPrimitiveClass);
    registerNativeUnlocked("java/lang/Class", "getGenericSignature0", "()Ljava/lang/String;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getRawAnnotations", "()[B", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getRawTypeAnnotations", "()[B", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getConstantPool", "()Ljdk/internal/reflect/ConstantPool;",
                           native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getDeclaredFields0", "(Z)[Ljava/lang/reflect/Field;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getDeclaredMethods0", "(Z)[Ljava/lang/reflect/Method;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getDeclaredConstructors0", "(Z)[Ljava/lang/reflect/Constructor;",
                           native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getDeclaredClasses0", "()[Ljava/lang/Class;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getRecordComponents0", "()[Ljava/lang/reflect/RecordComponent;",
                           native_ref_null);
    registerNativeUnlocked("java/lang/Class", "isRecord0", "()Z", native_Class_isRecord0);
    registerNativeUnlocked("java/lang/Class", "desiredAssertionStatus0", "(Ljava/lang/Class;)Z",
                           native_Class_desiredAssertionStatus0);
    registerNativeUnlocked("java/lang/Class", "getNestHost0", "()Ljava/lang/Class;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getNestMembers0", "()[Ljava/lang/Class;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "isHidden", "()Z", native_Class_isHidden);
    registerNativeUnlocked("java/lang/Class", "getPermittedSubclasses0", "()[Ljava/lang/Class;", native_ref_null);
    registerNativeUnlocked("java/lang/Class", "getClassFileVersion0", "()I", native_Class_getClassFileVersion0);
    registerNativeUnlocked("java/lang/Class", "getClassAccessFlagsRaw0", "()I", native_Class_getClassAccessFlagsRaw0);

    // java.lang.ClassLoader
    registerNativeUnlocked("java/lang/ClassLoader", "registerNatives", "()V", native_void_noop);

    // java.lang.Thread
    registerNativeUnlocked("java/lang/Thread", "registerNatives", "()V", native_Thread_registerNatives);
    registerNativeUnlocked("java/lang/Thread", "findScopedValueBindings", "()Ljava/lang/Object;", native_ref_null);
    registerNativeUnlocked("java/lang/Thread", "currentCarrierThread", "()Ljava/lang/Thread;",
                           native_Thread_currentCarrierThread);
    registerNativeUnlocked("java/lang/Thread", "currentThread", "()Ljava/lang/Thread;", native_Thread_currentThread);
    registerNativeUnlocked("java/lang/Thread", "setCurrentThread", "(Ljava/lang/Thread;)V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "scopedValueCache", "()[Ljava/lang/Object;", native_ref_null);
    registerNativeUnlocked("java/lang/Thread", "setScopedValueCache", "([Ljava/lang/Object;)V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "ensureMaterializedForStackWalk", "(Ljava/lang/Object;)V",
                           native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "yield0", "()V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "sleepNanos0", "(J)V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "start0", "()V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "holdsLock", "(Ljava/lang/Object;)Z", native_Thread_holdsLock);
    registerNativeUnlocked("java/lang/Thread", "getStackTrace0", "()Ljava/lang/Object;", native_ref_null);
    registerNativeUnlocked("java/lang/Thread", "dumpThreads", "([Ljava/lang/Thread;)[[Ljava/lang/StackTraceElement;",
                           native_ref_null);
    registerNativeUnlocked("java/lang/Thread", "getThreads", "()[Ljava/lang/Thread;", native_ref_null);
    registerNativeUnlocked("java/lang/Thread", "setPriority0", "(I)V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "clearInterruptEvent", "()V", native_Thread_clearInterruptEvent);
    registerNativeUnlocked("java/lang/Thread", "setNativeName", "(Ljava/lang/String;)V", native_void_noop);
    registerNativeUnlocked("java/lang/Thread", "getNextThreadIdOffset", "()J", native_Thread_getNextThreadIdOffset);
    registerNativeUnlocked("java/lang/Thread", "interrupt0", "()V", native_Thread_interrupt0);
    registerNativeUnlocked("java/lang/Thread", "isInterrupted", "(Z)Z", native_Thread_isInterrupted);
    registerNativeUnlocked("java/lang/Thread", "interrupted", "()Z", native_Thread_interrupted);

    // java.io.UnixFileSystem（POSIX 文件元数据 / 路径；见 FileNativeSupport.cpp）
    registerNativeUnlocked("java/io/UnixFileSystem", "initIDs", "()V", native_UnixFileSystem_initIDs);
    registerNativeUnlocked("java/io/UnixFileSystem", "canonicalize0", "(Ljava/lang/String;)Ljava/lang/String;",
                           native_UnixFileSystem_canonicalize0);
    registerNativeUnlocked("java/io/UnixFileSystem", "getBooleanAttributes0", "(Ljava/io/File;)I",
                           native_UnixFileSystem_getBooleanAttributes0);
    registerNativeUnlocked("java/io/UnixFileSystem", "checkAccess", "(Ljava/io/File;I)Z",
                           native_UnixFileSystem_checkAccess);
    registerNativeUnlocked("java/io/UnixFileSystem", "getLastModifiedTime", "(Ljava/io/File;)J",
                           native_UnixFileSystem_getLastModifiedTime);
    registerNativeUnlocked("java/io/UnixFileSystem", "getLength", "(Ljava/io/File;)J", native_UnixFileSystem_getLength);
    registerNativeUnlocked("java/io/UnixFileSystem", "setPermission", "(Ljava/io/File;IZZ)Z",
                           native_UnixFileSystem_setPermission);
    registerNativeUnlocked("java/io/UnixFileSystem", "createFileExclusively", "(Ljava/lang/String;)Z",
                           native_UnixFileSystem_createFileExclusively);
    registerNativeUnlocked("java/io/UnixFileSystem", "delete0", "(Ljava/io/File;)Z", native_UnixFileSystem_delete0);
    registerNativeUnlocked("java/io/UnixFileSystem", "list", "(Ljava/io/File;)[Ljava/lang/String;",
                           native_UnixFileSystem_list);
    registerNativeUnlocked("java/io/UnixFileSystem", "createDirectory", "(Ljava/io/File;)Z",
                           native_UnixFileSystem_createDirectory);
    registerNativeUnlocked("java/io/UnixFileSystem", "rename0", "(Ljava/io/File;Ljava/io/File;)Z",
                           native_UnixFileSystem_rename0);
    registerNativeUnlocked("java/io/UnixFileSystem", "setLastModifiedTime", "(Ljava/io/File;J)Z",
                           native_UnixFileSystem_setLastModifiedTime);
    registerNativeUnlocked("java/io/UnixFileSystem", "setReadOnly", "(Ljava/io/File;)Z",
                           native_UnixFileSystem_setReadOnly);
    registerNativeUnlocked("java/io/UnixFileSystem", "getSpace", "(Ljava/io/File;I)J", native_UnixFileSystem_getSpace);
    registerNativeUnlocked("java/io/UnixFileSystem", "getNameMax0", "(Ljava/lang/String;)J",
                           native_UnixFileSystem_getNameMax0);

    // java.net / java.io.FileDescriptor（套接字与解析；见 NetworkNativeSupport.cpp）
    registerNativeUnlocked("java/net/InetAddress", "init", "()V", native_InetAddress_init);
    registerNativeUnlocked("java/net/Inet4Address", "init", "()V", native_Inet4Address_init);
    registerNativeUnlocked("java/net/Inet6Address", "init", "()V", native_Inet6Address_init);

    registerNativeUnlocked("java/net/Inet4AddressImpl", "getLocalHostName", "()Ljava/lang/String;",
                           native_Inet4AddressImpl_getLocalHostName);
    registerNativeUnlocked("java/net/Inet4AddressImpl", "lookupAllHostAddr",
                           "(Ljava/lang/String;)[Ljava/net/InetAddress;", native_Inet4AddressImpl_lookupAllHostAddr);
    registerNativeUnlocked("java/net/Inet4AddressImpl", "getHostByAddr", "([B)Ljava/lang/String;",
                           native_Inet4AddressImpl_getHostByAddr);
    registerNativeUnlocked("java/net/Inet4AddressImpl", "isReachable0", "([BI[BI)Z",
                           native_Inet4AddressImpl_isReachable0);

    registerNativeUnlocked("java/net/Inet6AddressImpl", "getLocalHostName", "()Ljava/lang/String;",
                           native_Inet6AddressImpl_getLocalHostName);
    registerNativeUnlocked("java/net/Inet6AddressImpl", "lookupAllHostAddr",
                           "(Ljava/lang/String;)[Ljava/net/InetAddress;", native_Inet6AddressImpl_lookupAllHostAddr);
    registerNativeUnlocked("java/net/Inet6AddressImpl", "getHostByAddr", "([B)Ljava/lang/String;",
                           native_Inet6AddressImpl_getHostByAddr);
    registerNativeUnlocked("java/net/Inet6AddressImpl", "isReachable0", "([BII[BII)Z",
                           native_Inet6AddressImpl_isReachable0);

    registerNativeUnlocked("java/net/PlainSocketImpl", "socketCreate", "(ZZ)V", native_PlainSocketImpl_socketCreate);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketConnect", "(Ljava/net/InetAddress;II)V",
                           native_PlainSocketImpl_socketConnect);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketBind", "(Ljava/net/InetAddress;I)V",
                           native_PlainSocketImpl_socketBind);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketListen", "(I)V", native_PlainSocketImpl_socketListen);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketAccept", "(Ljava/net/SocketImpl;)V",
                           native_PlainSocketImpl_socketAccept);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketAvailable", "()I", native_PlainSocketImpl_socketAvailable);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketClose0", "(Z)V", native_PlainSocketImpl_socketClose0);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketShutdown", "(I)V", native_PlainSocketImpl_socketShutdown);
    registerNativeUnlocked("java/net/PlainSocketImpl", "initProto", "()V", native_PlainSocketImpl_initProto);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketSetOption0", "(IZLjava/lang/Object;)V",
                           native_PlainSocketImpl_socketSetOption0);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketGetOption", "(ILjava/lang/Object;)I",
                           native_PlainSocketImpl_socketGetOption);
    registerNativeUnlocked("java/net/PlainSocketImpl", "socketSendUrgentData", "(I)V",
                           native_PlainSocketImpl_socketSendUrgentData);

    registerNativeUnlocked("java/net/SocketInputStream", "socketRead0", "(Ljava/io/FileDescriptor;[BIII)I",
                           native_SocketInputStream_socketRead0);
    registerNativeUnlocked("java/net/SocketInputStream", "init", "()V", native_SocketInputStream_init);

    registerNativeUnlocked("java/net/SocketOutputStream", "socketWrite0", "(Ljava/io/FileDescriptor;[BII)V",
                           native_SocketOutputStream_socketWrite0);
    registerNativeUnlocked("java/net/SocketOutputStream", "init", "()V", native_SocketOutputStream_init);

    registerNativeUnlocked("java/net/NetworkInterface", "init", "()V", native_NetworkInterface_init);
    registerNativeUnlocked("java/net/NetworkInterface", "getAll", "()[Ljava/net/NetworkInterface;",
                           native_NetworkInterface_getAll);
    registerNativeUnlocked("java/net/NetworkInterface", "getByName0", "(Ljava/lang/String;)Ljava/net/NetworkInterface;",
                           native_NetworkInterface_getByName0);
    registerNativeUnlocked("java/net/NetworkInterface", "getByIndex0", "(I)Ljava/net/NetworkInterface;",
                           native_NetworkInterface_getByIndex0);
    registerNativeUnlocked("java/net/NetworkInterface", "boundInetAddress0", "(Ljava/net/InetAddress;)Z",
                           native_NetworkInterface_boundInetAddress0);
    registerNativeUnlocked("java/net/NetworkInterface", "getByInetAddress0",
                           "(Ljava/net/InetAddress;)Ljava/net/NetworkInterface;",
                           native_NetworkInterface_getByInetAddress0);
    registerNativeUnlocked("java/net/NetworkInterface", "isUp0", "(Ljava/lang/String;I)Z",
                           native_NetworkInterface_isUp0);
    registerNativeUnlocked("java/net/NetworkInterface", "isLoopback0", "(Ljava/lang/String;I)Z",
                           native_NetworkInterface_isLoopback0);
    registerNativeUnlocked("java/net/NetworkInterface", "supportsMulticast0", "(Ljava/lang/String;I)Z",
                           native_NetworkInterface_supportsMulticast0);
    registerNativeUnlocked("java/net/NetworkInterface", "isP2P0", "(Ljava/lang/String;I)Z",
                           native_NetworkInterface_isP2P0);
    registerNativeUnlocked("java/net/NetworkInterface", "getMacAddr0", "([BLjava/lang/String;I)[B",
                           native_NetworkInterface_getMacAddr0);
    registerNativeUnlocked("java/net/NetworkInterface", "getMTU0", "(Ljava/lang/String;I)I",
                           native_NetworkInterface_getMTU0);

    registerNativeUnlocked("sun/net/spi/DefaultProxySelector", "init", "()Z", native_DefaultProxySelector_init);
    registerNativeUnlocked("sun/net/spi/DefaultProxySelector", "getSystemProxies",
                           "(Ljava/lang/String;Ljava/lang/String;)[Ljava/net/Proxy;",
                           native_DefaultProxySelector_getSystemProxies);

    registerNativeUnlocked("sun/net/dns/ResolverConfigurationImpl", "fallbackDomain0", "()Ljava/lang/String;",
                           native_ResolverConfigurationImpl_fallbackDomain0);

    // sun.nio.ch — JDK 默认 NioSocketImpl 依赖（与 OpenJDK Net.c / SocketDispatcher.c 语义对齐）
    registerNativeUnlocked("sun/nio/ch/Net", "initIDs", "()V", native_Net_initIDs);
    registerNativeUnlocked("sun/nio/ch/Net", "isIPv6Available0", "()Z", native_Net_isIPv6Available0);
    registerNativeUnlocked("sun/nio/ch/Net", "isReusePortAvailable0", "()Z", native_Net_isReusePortAvailable0);
    registerNativeUnlocked("sun/nio/ch/Net", "isExclusiveBindAvailable", "()I", native_Net_isExclusiveBindAvailable);
    registerNativeUnlocked("sun/nio/ch/Net", "shouldSetBothIPv4AndIPv6Options0", "()Z",
                           native_Net_shouldSetBothIPv4AndIPv6Options0);
    registerNativeUnlocked("sun/nio/ch/Net", "canIPv6SocketJoinIPv4Group0", "()Z",
                           native_Net_canIPv6SocketJoinIPv4Group0);
    registerNativeUnlocked("sun/nio/ch/Net", "canJoin6WithIPv4Group0", "()Z", native_Net_canJoin6WithIPv4Group0);
    registerNativeUnlocked("sun/nio/ch/Net", "canUseIPv6OptionsWithIPv4LocalAddress0", "()Z",
                           native_Net_canUseIPv6OptionsWithIPv4LocalAddress0);
    registerNativeUnlocked("sun/nio/ch/Net", "socket0", "(ZZZZ)I", native_Net_socket0);
    registerNativeUnlocked("sun/nio/ch/Net", "bind0", "(Ljava/io/FileDescriptor;ZZLjava/net/InetAddress;I)V",
                           native_Net_bind0);
    registerNativeUnlocked("sun/nio/ch/Net", "listen", "(Ljava/io/FileDescriptor;I)V", native_Net_listen);
    registerNativeUnlocked("sun/nio/ch/Net", "connect0", "(ZLjava/io/FileDescriptor;Ljava/net/InetAddress;I)I",
                           native_Net_connect0);
    registerNativeUnlocked("sun/nio/ch/Net", "accept",
                           "(Ljava/io/FileDescriptor;Ljava/io/FileDescriptor;[Ljava/net/InetSocketAddress;)I",
                           native_Net_accept);
    registerNativeUnlocked("sun/nio/ch/Net", "shutdown", "(Ljava/io/FileDescriptor;I)V", native_Net_shutdown);
    registerNativeUnlocked("sun/nio/ch/Net", "localPort", "(Ljava/io/FileDescriptor;)I", native_Net_localPort);
    registerNativeUnlocked("sun/nio/ch/Net", "localInetAddress", "(Ljava/io/FileDescriptor;)Ljava/net/InetAddress;",
                           native_Net_localInetAddress);
    registerNativeUnlocked("sun/nio/ch/Net", "remotePort", "(Ljava/io/FileDescriptor;)I", native_Net_remotePort);
    registerNativeUnlocked("sun/nio/ch/Net", "remoteInetAddress", "(Ljava/io/FileDescriptor;)Ljava/net/InetAddress;",
                           native_Net_remoteInetAddress);
    registerNativeUnlocked("sun/nio/ch/Net", "getIntOption0", "(Ljava/io/FileDescriptor;ZII)I",
                           native_Net_getIntOption0);
    registerNativeUnlocked("sun/nio/ch/Net", "setIntOption0", "(Ljava/io/FileDescriptor;ZIIIZ)V",
                           native_Net_setIntOption0);
    registerNativeUnlocked("sun/nio/ch/Net", "poll", "(Ljava/io/FileDescriptor;IJ)I", native_Net_poll);
    registerNativeUnlocked("sun/nio/ch/Net", "pollConnect", "(Ljava/io/FileDescriptor;J)Z", native_Net_pollConnect);
    registerNativeUnlocked("sun/nio/ch/Net", "available", "(Ljava/io/FileDescriptor;)I", native_Net_available);
    registerNativeUnlocked("sun/nio/ch/Net", "sendOOB", "(Ljava/io/FileDescriptor;B)I", native_Net_sendOOB);
    registerNativeUnlocked("sun/nio/ch/Net", "discardOOB", "(Ljava/io/FileDescriptor;)Z", native_Net_discardOOB);
    registerNativeUnlocked("sun/nio/ch/Net", "joinOrDrop4", "(ZLjava/io/FileDescriptor;III)I",
                           native_Net_joinOrDrop4);
    registerNativeUnlocked("sun/nio/ch/Net", "blockOrUnblock4", "(ZLjava/io/FileDescriptor;III)I",
                           native_Net_blockOrUnblock4);
    registerNativeUnlocked("sun/nio/ch/Net", "joinOrDrop6", "(ZLjava/io/FileDescriptor;[BI[B)I",
                           native_Net_joinOrDrop6);
    registerNativeUnlocked("sun/nio/ch/Net", "blockOrUnblock6", "(ZLjava/io/FileDescriptor;[BI[B)I",
                           native_Net_blockOrUnblock6);
    registerNativeUnlocked("sun/nio/ch/Net", "setInterface4", "(Ljava/io/FileDescriptor;I)V",
                           native_Net_setInterface4);
    registerNativeUnlocked("sun/nio/ch/Net", "getInterface4", "(Ljava/io/FileDescriptor;)I",
                           native_Net_getInterface4);
    registerNativeUnlocked("sun/nio/ch/Net", "setInterface6", "(Ljava/io/FileDescriptor;I)V",
                           native_Net_setInterface6);
    registerNativeUnlocked("sun/nio/ch/Net", "getInterface6", "(Ljava/io/FileDescriptor;)I",
                           native_Net_getInterface6);
    registerNativeUnlocked("sun/nio/ch/Net", "pollinValue", "()S", native_Net_pollinValue);
    registerNativeUnlocked("sun/nio/ch/Net", "polloutValue", "()S", native_Net_polloutValue);
    registerNativeUnlocked("sun/nio/ch/Net", "pollerrValue", "()S", native_Net_pollerrValue);
    registerNativeUnlocked("sun/nio/ch/Net", "pollhupValue", "()S", native_Net_pollhupValue);
    registerNativeUnlocked("sun/nio/ch/Net", "pollnvalValue", "()S", native_Net_pollnvalValue);
    registerNativeUnlocked("sun/nio/ch/Net", "pollconnValue", "()S", native_Net_pollconnValue);

    registerNativeUnlocked("sun/nio/ch/SocketDispatcher", "read0", "(Ljava/io/FileDescriptor;JI)I",
                           native_SocketDispatcher_read0);
    registerNativeUnlocked("sun/nio/ch/SocketDispatcher", "readv0", "(Ljava/io/FileDescriptor;JI)J",
                           native_SocketDispatcher_readv0);
    registerNativeUnlocked("sun/nio/ch/SocketDispatcher", "write0", "(Ljava/io/FileDescriptor;JI)I",
                           native_SocketDispatcher_write0);
    registerNativeUnlocked("sun/nio/ch/SocketDispatcher", "writev0", "(Ljava/io/FileDescriptor;JI)J",
                           native_SocketDispatcher_writev0);

    for (const char* unsafeCls : {"jdk/internal/misc/Unsafe", "sun/misc/Unsafe"}) {
        const std::string unsafeClass(unsafeCls);
        registerNativeUnlocked(unsafeClass, "registerNatives", "()V", native_Unsafe_registerNatives);

        registerNativeUnlocked(unsafeClass, "getInt", "(Ljava/lang/Object;J)I", native_Unsafe_getInt);
        registerNativeUnlocked(unsafeClass, "putInt", "(Ljava/lang/Object;JI)V", native_Unsafe_putInt);
        registerNativeUnlocked(unsafeClass, "getIntVolatile", "(Ljava/lang/Object;J)I", native_Unsafe_getIntVolatile);
        registerNativeUnlocked(unsafeClass, "putIntVolatile", "(Ljava/lang/Object;JI)V", native_Unsafe_putIntVolatile);

        registerNativeUnlocked(unsafeClass, "getLong", "(Ljava/lang/Object;J)J", native_Unsafe_getLong);
        registerNativeUnlocked(unsafeClass, "putLong", "(Ljava/lang/Object;JJ)V", native_Unsafe_putLong);
        registerNativeUnlocked(unsafeClass, "getLongVolatile", "(Ljava/lang/Object;J)J", native_Unsafe_getLongVolatile);
        registerNativeUnlocked(unsafeClass, "putLongVolatile", "(Ljava/lang/Object;JJ)V", native_Unsafe_putLongVolatile);

        registerNativeUnlocked(unsafeClass, "getFloat", "(Ljava/lang/Object;J)F", native_Unsafe_getFloat);
        registerNativeUnlocked(unsafeClass, "putFloat", "(Ljava/lang/Object;JF)V", native_Unsafe_putFloat);
        registerNativeUnlocked(unsafeClass, "getFloatVolatile", "(Ljava/lang/Object;J)F", native_Unsafe_getFloatVolatile);
        registerNativeUnlocked(unsafeClass, "putFloatVolatile", "(Ljava/lang/Object;JF)V", native_Unsafe_putFloatVolatile);

        registerNativeUnlocked(unsafeClass, "getDouble", "(Ljava/lang/Object;J)D", native_Unsafe_getDouble);
        registerNativeUnlocked(unsafeClass, "putDouble", "(Ljava/lang/Object;JD)V", native_Unsafe_putDouble);
        registerNativeUnlocked(unsafeClass, "getDoubleVolatile", "(Ljava/lang/Object;J)D", native_Unsafe_getDoubleVolatile);
        registerNativeUnlocked(unsafeClass, "putDoubleVolatile", "(Ljava/lang/Object;JD)V", native_Unsafe_putDoubleVolatile);

        registerNativeUnlocked(unsafeClass, "getReference", "(Ljava/lang/Object;J)Ljava/lang/Object;", native_Unsafe_getReference);
        registerNativeUnlocked(unsafeClass, "putReference", "(Ljava/lang/Object;JLjava/lang/Object;)V", native_Unsafe_putReference);
        registerNativeUnlocked(unsafeClass, "getReferenceVolatile", "(Ljava/lang/Object;J)Ljava/lang/Object;",
                           native_Unsafe_getReferenceVolatile);
        registerNativeUnlocked(unsafeClass, "putReferenceVolatile", "(Ljava/lang/Object;JLjava/lang/Object;)V",
                           native_Unsafe_putReferenceVolatile);

        registerNativeUnlocked(unsafeClass, "getBoolean", "(Ljava/lang/Object;J)Z", native_Unsafe_getBoolean);
        registerNativeUnlocked(unsafeClass, "putBoolean", "(Ljava/lang/Object;JZ)V", native_Unsafe_putBoolean);
        registerNativeUnlocked(unsafeClass, "getBooleanVolatile", "(Ljava/lang/Object;J)Z", native_Unsafe_getBooleanVolatile);
        registerNativeUnlocked(unsafeClass, "putBooleanVolatile", "(Ljava/lang/Object;JZ)V", native_Unsafe_putBooleanVolatile);

        registerNativeUnlocked(unsafeClass, "getByte", "(Ljava/lang/Object;J)B", native_Unsafe_getByte);
        registerNativeUnlocked(unsafeClass, "putByte", "(Ljava/lang/Object;JB)V", native_Unsafe_putByte);
        registerNativeUnlocked(unsafeClass, "getByteVolatile", "(Ljava/lang/Object;J)B", native_Unsafe_getByteVolatile);
        registerNativeUnlocked(unsafeClass, "putByteVolatile", "(Ljava/lang/Object;JB)V", native_Unsafe_putByteVolatile);

        registerNativeUnlocked(unsafeClass, "getShort", "(Ljava/lang/Object;J)S", native_Unsafe_getShort);
        registerNativeUnlocked(unsafeClass, "putShort", "(Ljava/lang/Object;JS)V", native_Unsafe_putShort);
        registerNativeUnlocked(unsafeClass, "getShortVolatile", "(Ljava/lang/Object;J)S", native_Unsafe_getShortVolatile);
        registerNativeUnlocked(unsafeClass, "putShortVolatile", "(Ljava/lang/Object;JS)V", native_Unsafe_putShortVolatile);

        registerNativeUnlocked(unsafeClass, "getChar", "(Ljava/lang/Object;J)C", native_Unsafe_getChar);
        registerNativeUnlocked(unsafeClass, "putChar", "(Ljava/lang/Object;JC)V", native_Unsafe_putChar);
        registerNativeUnlocked(unsafeClass, "getCharVolatile", "(Ljava/lang/Object;J)C", native_Unsafe_getCharVolatile);
        registerNativeUnlocked(unsafeClass, "putCharVolatile", "(Ljava/lang/Object;JC)V", native_Unsafe_putCharVolatile);

        registerNativeUnlocked(unsafeClass, "knownObjectFieldOffset0", "(Ljava/lang/Class;Ljava/lang/String;)J",
                           native_Unsafe_objectFieldOffset1);

        registerNativeUnlocked(unsafeClass, "allocateMemory0", "(J)J", native_Unsafe_allocateMemory0);
        registerNativeUnlocked(unsafeClass, "reallocateMemory0", "(JJ)J", native_Unsafe_reallocateMemory0);
        registerNativeUnlocked(unsafeClass, "freeMemory0", "(J)V", native_Unsafe_freeMemory0);
        registerNativeUnlocked(unsafeClass, "setMemory0", "(Ljava/lang/Object;JJB)V",
                           native_Unsafe_setMemory0);
        registerNativeUnlocked(unsafeClass, "copyMemory0",
                           "(Ljava/lang/Object;JLjava/lang/Object;JJ)V", native_Unsafe_copyMemory0);
        registerNativeUnlocked(unsafeClass, "fullFence", "()V", native_Unsafe_fullFence);
        registerNativeUnlocked(unsafeClass, "loadFence", "()V", native_Unsafe_loadFence);
        registerNativeUnlocked(unsafeClass, "storeFence", "()V", native_Unsafe_storeFence);
        registerNativeUnlocked(unsafeClass, "compareAndSetReference",
                           "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z",
                           native_Unsafe_compareAndSetReference);
        registerNativeUnlocked(unsafeClass, "compareAndExchangeReference",
                           "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;",
                           native_Unsafe_compareAndExchangeReference);
        registerNativeUnlocked(unsafeClass, "compareAndSetInt", "(Ljava/lang/Object;JII)Z",
                           native_Unsafe_compareAndSetInt);
        registerNativeUnlocked(unsafeClass, "compareAndExchangeInt", "(Ljava/lang/Object;JII)I",
                           native_Unsafe_compareAndExchangeInt);
        registerNativeUnlocked(unsafeClass, "compareAndSetLong", "(Ljava/lang/Object;JJJ)Z",
                           native_Unsafe_compareAndSetLong);
        registerNativeUnlocked(unsafeClass, "compareAndExchangeLong", "(Ljava/lang/Object;JJJ)J",
                           native_Unsafe_compareAndExchangeLong);
        registerNativeUnlocked(unsafeClass, "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J",
                           native_Unsafe_objectFieldOffset0);
        registerNativeUnlocked(unsafeClass, "objectFieldOffset1", "(Ljava/lang/Class;Ljava/lang/String;)J",
                           native_Unsafe_objectFieldOffset1);
        registerNativeUnlocked(unsafeClass, "staticFieldOffset0", "(Ljava/lang/reflect/Field;)J",
                           native_Unsafe_staticFieldOffset0);
        registerNativeUnlocked(unsafeClass, "staticFieldBase0", "(Ljava/lang/reflect/Field;)Ljava/lang/Object;",
                           native_Unsafe_staticFieldBase0);
        registerNativeUnlocked(unsafeClass, "arrayBaseOffset0", "(Ljava/lang/Class;)I",
                           native_Unsafe_arrayBaseOffset0);
        registerNativeUnlocked(unsafeClass, "arrayIndexScale0", "(Ljava/lang/Class;)I",
                           native_Unsafe_arrayIndexScale0);
        registerNativeUnlocked(unsafeClass, "shouldBeInitialized0", "(Ljava/lang/Class;)Z",
                           native_Unsafe_shouldBeInitialized0);
        registerNativeUnlocked(unsafeClass, "ensureClassInitialized0", "(Ljava/lang/Class;)V",
                           native_Unsafe_ensureClassInitialized0);
        registerNativeUnlocked(unsafeClass, "copySwapMemory0",
                           "(Ljava/lang/Object;JLjava/lang/Object;JJJ)V", native_Unsafe_copySwapMemory0);
        registerNativeUnlocked(unsafeClass, "writeback0", "(J)V", native_Unsafe_writeback0);
        registerNativeUnlocked(unsafeClass, "writebackPreSync0", "()V", native_Unsafe_writebackPreSync0);
        registerNativeUnlocked(unsafeClass, "writebackPostSync0", "()V", native_Unsafe_writebackPostSync0);
        registerNativeUnlocked(unsafeClass, "getLoadAverage0", "([DI)I", native_Unsafe_getLoadAverage0);

        registerNativeUnlocked(unsafeClass, "park", "(ZJ)V", native_Unsafe_park);
        registerNativeUnlocked(unsafeClass, "unpark", "(Ljava/lang/Object;)V", native_Unsafe_unpark);
        registerNativeUnlocked(unsafeClass, "allocateInstance", "(Ljava/lang/Class;)Ljava/lang/Object;",
                           native_Unsafe_allocateInstance);
        registerNativeUnlocked(unsafeClass, "throwException", "(Ljava/lang/Throwable;)V",
                           native_Unsafe_throwException);
        registerNativeUnlocked(unsafeClass, "invokeCleaner", "(Ljava/nio/ByteBuffer;)V", native_Unsafe_invokeCleaner);
        registerNativeUnlocked(unsafeClass, "defineClass0",
                           "(Ljava/lang/String;[BIILjava/lang/ClassLoader;Ljava/security/ProtectionDomain;)Ljava/lang/Class;",
                           native_Unsafe_defineClass0);
        registerNativeUnlocked(unsafeClass, "getUncompressedObject", "(J)Ljava/lang/Object;",
                           native_Unsafe_getUncompressedObject);
    }

    registerNativeUnlocked("java/io/FileDescriptor", "initIDs", "()V", native_FileDescriptor_initIDs);
    registerNativeUnlocked("java/io/FileDescriptor", "sync", "()V", native_FileDescriptor_sync);
    registerNativeUnlocked("java/io/FileDescriptor", "getHandle", "(I)J", native_FileDescriptor_getHandle);
    registerNativeUnlocked("java/io/FileDescriptor", "getAppend", "(I)Z", native_FileDescriptor_getAppend);
    registerNativeUnlocked("java/io/FileDescriptor", "close0", "()V", native_FileDescriptor_close0);
}

bool NativeMethodRegistry::registerNative(const std::string& classInternalName,
                                          const std::string& methodName,
                                          const std::string& descriptor,
                                          NativeMethodHandler handler) {
    if (!handler) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mu_);
    const std::string key = makeKey(classInternalName, methodName, descriptor);
    if (handlers_.find(key) != handlers_.end()) {
        return false;
    }
    handlers_[key] = handler;
    return true;
}

bool NativeMethodRegistry::tryInvoke(JavaFrame* caller, JavaFrame* callee, Heap::Method* method) {
    if (!method || !method->isNative()) {
        return false;
    }
    Runtime::JavaClass* jc = method->getJavaClass();
    if (!jc) {
        return false;
    }
    const std::string& cls = jc->getThisClassName();
    const std::string key = makeKey(cls, method->getName(), method->getDescriptor());

    NativeMethodHandler fn = nullptr;
    {
        std::lock_guard<std::mutex> lock(mu_);
        const auto it = handlers_.find(key);
        if (it == handlers_.end()) {
            return false;
        }
        fn = it->second;
    }

    NativeCallContext ctx{caller, callee, method};
    fn(ctx);
    return true;
}

void NativeMethodRegistry::resetForTesting() {
    {
        std::lock_guard<std::mutex> lock(mu_);
        handlers_.clear();
    }
    std::lock_guard<std::mutex> auxLock(g_auxMutex);
    clearAuxStateLocked();
}

void NativeMethodRegistry::installBuiltInNatives() {
    std::lock_guard<std::mutex> lock(mu_);
    registerBuiltInsUnlocked();
}

void registerBuiltInNatives() {
    NativeMethodRegistry::instance().installBuiltInNatives();
}

} // namespace Native
} // namespace Runtime
