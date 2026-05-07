//
// jdk.internal.misc.Unsafe：堆外内存、get*/put*（含 volatile）、CAS、fence、字段/数组 offset、类初始化等。
// 与 HotSpot 常用子集对齐：o==null 为绝对地址；数组为 oop + byteOffset（与 arrayBaseOffset0 配套）；
// java/lang/Class 镜像 + offset 为静态槽；其它实例对象 offset 为实例槽下标（Field slotId）。
//

#include "UnsafeNativeSupport.h"

#include "NativeMethodRegistry.h"

#include "../ExceptionHelpers.h"
#include "../JvmThrownObject.h"
#include "../ClassLoader.h"
#include "../JavaThread.h"
#include "../LocalVariableTables.h"
#include "../Object.h"
#include "../OperandStack.h"
#include "../detail/ArrayCoallocLayout.h"
#include "../heap/ClassMember.h"
#include "../heap/JavaClass.h"
#include "../heap/StringConstantPools.h"
#include "../../lang/ByteQueue.h"

#include <atomic>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace Runtime {
namespace Native {

namespace {

using Heap::FieldInfo;

size_t unsafePrimArrayElemBytes(const std::string& n) {
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

size_t arrayPayloadSpanBytes(Object* obj, JavaClass* jc) {
    const int alen = obj->getArrayLength();
    if (alen < 0 || !jc) {
        return 0;
    }
    const std::string& name = jc->getThisClassName();
    if (name == "[B") {
        return static_cast<size_t>(alen);
    }
    if (name.size() >= 2 && name[0] == '[' && (name[1] == 'L' || name[1] == '[')) {
        return static_cast<size_t>(alen) * sizeof(Object*);
    }
    const size_t esz = unsafePrimArrayElemBytes(name);
    if (esz == 0) {
        return 0;
    }
    return static_cast<size_t>(alen) * esz;
}

/** o==null 为绝对地址；数组为 (oop+offset) 且落在 payload 区内。 */
unsigned char* unsafeResolveArrayOrAbsolute(Object* obj, long offset, size_t width) {
    if (!obj) {
        if (offset < 0) {
            return nullptr;
        }
        return reinterpret_cast<unsigned char*>(static_cast<uintptr_t>(offset));
    }
    if (obj->getArrayLength() < 0) {
        return nullptr;
    }
    JavaClass* jc = obj->getJavaClass();
    if (!jc) {
        return nullptr;
    }
    if (offset < 0) {
        return nullptr;
    }
    const auto offu = static_cast<uintptr_t>(offset);
    unsigned char* target = reinterpret_cast<unsigned char*>(static_cast<void*>(obj)) + offu;
    unsigned char* d0 = reinterpret_cast<unsigned char*>(obj->getData());
    const size_t span = arrayPayloadSpanBytes(obj, jc);
    if (target < d0) {
        return nullptr;
    }
    const size_t rel = static_cast<size_t>(target - d0);
    if (rel + width > span || rel + width < rel) {
        return nullptr;
    }
    return target;
}

Slots* unsafeResolveStaticVarSlot(Object* obj, long offset) {
    if (!obj || obj->getArrayLength() >= 0) {
        return nullptr;
    }
    JavaClass* meta = obj->getJavaClass();
    if (!meta || meta->getThisClassName() != "java/lang/Class") {
        return nullptr;
    }
    JavaClass* jc = javaClassFromClassMirrorObject(obj);
    if (!jc) {
        return nullptr;
    }
    if (offset < 0 || offset > static_cast<long>(INT_MAX)) {
        return nullptr;
    }
    const int idx = static_cast<int>(offset);
    if (static_cast<long>(idx) != offset) {
        return nullptr;
    }
    const int n = jc->getStaticCount();
    if (idx < 0 || idx >= n) {
        return nullptr;
    }
    const std::vector<Slots*>& vec = jc->getStaticVars();
    if (static_cast<size_t>(idx) >= vec.size()) {
        return nullptr;
    }
    return vec[static_cast<size_t>(idx)];
}

Slots* unsafeResolveInstanceSlot(Object* obj, long offset) {
    if (!obj || obj->getArrayLength() >= 0) {
        return nullptr;
    }
    JavaClass* meta = obj->getJavaClass();
    if (meta && meta->getThisClassName() == "java/lang/Class") {
        return nullptr;
    }
    JavaClass* jc = obj->getJavaClass();
    if (!jc) {
        return nullptr;
    }
    Slots* fields = obj->getFields();
    if (!fields) {
        return nullptr;
    }
    const int n = static_cast<int>(jc->getInstanceCount());
    if (offset < 0 || offset > static_cast<long>(INT_MAX)) {
        return nullptr;
    }
    const int idx = static_cast<int>(offset);
    if (static_cast<long>(idx) != offset) {
        return nullptr;
    }
    if (idx < 0 || idx >= n) {
        return nullptr;
    }
    return &fields[idx];
}

template <typename T>
bool isAlignedForAtomic(const void* p) {
    return (reinterpret_cast<uintptr_t>(p) % alignof(std::atomic<T>)) == 0;
}

ClassLoader* loaderFromCalleeUnsafe(const NativeCallContext& ctx) {
    if (!ctx.callee || !ctx.callee->getMethod() || !ctx.callee->getMethod()->getJavaClass()) {
        return nullptr;
    }
    return ctx.callee->getMethod()->getJavaClass()->getClassLoader();
}

/** 与 NativeMethodRegistry 中 binaryNameToInternal 一致：供 defineClass0 校验名称。 */
static std::string binaryNameToInternalForDefine(const std::string& binary) {
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

FieldInfo* findInstanceFieldByName(JavaClass* start, const std::string& name) {
    for (JavaClass* jc = start; jc; jc = jc->getSuperClass()) {
        for (FieldInfo* f : jc->getFieldInfoList()) {
            if (!f->isStatic() && f->getName() == name) {
                return f;
            }
        }
    }
    return nullptr;
}

FieldInfo* findStaticFieldByName(JavaClass* start, const std::string& name) {
    for (JavaClass* jc = start; jc; jc = jc->getSuperClass()) {
        for (FieldInfo* f : jc->getFieldInfoList()) {
            if (f->isStatic() && f->getName() == name) {
                return f;
            }
        }
    }
    return nullptr;
}

/** 将长度为 elem 的原子单元按字节序反转后写入 dst（与 copySwapMemory 语义一致）。 */
void swapElemTo(void* dst, const void* src, size_t elem) {
    const auto* s = static_cast<const unsigned char*>(src);
    auto* d = static_cast<unsigned char*>(dst);
    for (size_t i = 0; i < elem; ++i) {
        d[i] = s[elem - 1U - i];
    }
}

void copySwapMemorySpan(unsigned char* dst, unsigned char* src, size_t nbytes, size_t elem) {
    if (elem == 0 || nbytes % elem != 0) {
        return;
    }
    for (size_t i = 0; i < nbytes; i += elem) {
        swapElemTo(dst + i, src + i, elem);
    }
}

const FieldInfo* findInstanceFieldBySlot(JavaClass* jc, int idx) {
    for (JavaClass* c = jc; c; c = c->getSuperClass()) {
        for (FieldInfo* f : c->getFieldInfoList()) {
            if (!f->isStatic() && f->getSlotId() == idx) {
                return f;
            }
        }
    }
    return nullptr;
}

const FieldInfo* findStaticFieldBySlot(JavaClass* jc, int idx) {
    for (FieldInfo* f : jc->getFieldInfoList()) {
        if (f->isStatic() && f->getSlotId() == idx) {
            return f;
        }
    }
    return nullptr;
}

static char fieldDesc0(const FieldInfo* fi) {
    if (!fi || fi->getDescriptor().empty()) {
        return 'I';
    }
    return fi->getDescriptor()[0];
}

template <bool Volatile>
static int loadRawInt(const unsigned char* p) {
    auto* mut = const_cast<unsigned char*>(p);
    int v = 0;
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int>(mut)) {
            v = reinterpret_cast<std::atomic<int>*>(mut)->load(std::memory_order_acquire);
        } else {
            std::atomic_thread_fence(std::memory_order_acquire);
            std::memcpy(&v, p, sizeof(v));
        }
    } else {
        std::memcpy(&v, p, sizeof(v));
    }
    return v;
}

template <bool Volatile>
static void storeRawInt(unsigned char* p, int v) {
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int>(p)) {
            reinterpret_cast<std::atomic<int>*>(p)->store(v, std::memory_order_release);
        } else {
            std::atomic_thread_fence(std::memory_order_release);
            std::memcpy(p, &v, sizeof(v));
        }
    } else {
        std::memcpy(p, &v, sizeof(v));
    }
}

template <bool Volatile>
static long loadRawLong(const unsigned char* p) {
    int64_t v = 0;
    auto* mut = const_cast<unsigned char*>(p);
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int64_t>(mut)) {
            v = reinterpret_cast<std::atomic<int64_t>*>(mut)->load(std::memory_order_acquire);
        } else {
            std::atomic_thread_fence(std::memory_order_acquire);
            std::memcpy(&v, p, sizeof(v));
        }
    } else {
        std::memcpy(&v, p, sizeof(v));
    }
    return static_cast<long>(v);
}

template <bool Volatile>
static void storeRawLong(unsigned char* p, long v) {
    const int64_t w = static_cast<int64_t>(v);
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int64_t>(p)) {
            reinterpret_cast<std::atomic<int64_t>*>(p)->store(w, std::memory_order_release);
        } else {
            std::atomic_thread_fence(std::memory_order_release);
            std::memcpy(p, &w, sizeof(w));
        }
    } else {
        std::memcpy(p, &w, sizeof(w));
    }
}

template <bool Volatile>
static float loadRawFloat(const unsigned char* p) {
    float v = 0.f;
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int>(p)) {
            const int bits = reinterpret_cast<const std::atomic<int>*>(p)->load(std::memory_order_acquire);
            std::memcpy(&v, &bits, sizeof(v));
        } else {
            std::atomic_thread_fence(std::memory_order_acquire);
            std::memcpy(&v, p, sizeof(v));
        }
    } else {
        std::memcpy(&v, p, sizeof(v));
    }
    return v;
}

template <bool Volatile>
static void storeRawFloat(unsigned char* p, float v) {
    int bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int>(p)) {
            reinterpret_cast<std::atomic<int>*>(p)->store(bits, std::memory_order_release);
        } else {
            std::atomic_thread_fence(std::memory_order_release);
            std::memcpy(p, &v, sizeof(v));
        }
    } else {
        std::memcpy(p, &v, sizeof(v));
    }
}

template <bool Volatile>
static double loadRawDouble(const unsigned char* p) {
    double v = 0.0;
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int64_t>(p)) {
            const int64_t bits =
                reinterpret_cast<const std::atomic<int64_t>*>(p)->load(std::memory_order_acquire);
            std::memcpy(&v, &bits, sizeof(v));
        } else {
            std::atomic_thread_fence(std::memory_order_acquire);
            std::memcpy(&v, p, sizeof(v));
        }
    } else {
        std::memcpy(&v, p, sizeof(v));
    }
    return v;
}

template <bool Volatile>
static void storeRawDouble(unsigned char* p, double v) {
    int64_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    if constexpr (Volatile) {
        if (isAlignedForAtomic<int64_t>(p)) {
            reinterpret_cast<std::atomic<int64_t>*>(p)->store(bits, std::memory_order_release);
        } else {
            std::atomic_thread_fence(std::memory_order_release);
            std::memcpy(p, &v, sizeof(v));
        }
    } else {
        std::memcpy(p, &v, sizeof(v));
    }
}

template <bool Volatile>
static Object* loadRawRef(unsigned char* p) {
    constexpr size_t kPtr = sizeof(Object*);
    auto* mut = const_cast<unsigned char*>(p);
    if constexpr (Volatile) {
        if (isAlignedForAtomic<uintptr_t>(mut)) {
            const uintptr_t u =
                reinterpret_cast<std::atomic<uintptr_t>*>(mut)->load(std::memory_order_acquire);
            return reinterpret_cast<Object*>(u);
        }
        std::atomic_thread_fence(std::memory_order_acquire);
        Object* r = nullptr;
        std::memcpy(&r, p, kPtr);
        return r;
    }
    Object* r = nullptr;
    std::memcpy(&r, p, kPtr);
    return r;
}

template <bool Volatile>
static void storeRawRef(unsigned char* p, Object* r) {
    constexpr size_t kPtr = sizeof(Object*);
    const uintptr_t u = reinterpret_cast<uintptr_t>(r);
    if constexpr (Volatile) {
        if (isAlignedForAtomic<uintptr_t>(p)) {
            reinterpret_cast<std::atomic<uintptr_t>*>(p)->store(u, std::memory_order_release);
        } else {
            std::atomic_thread_fence(std::memory_order_release);
            std::memcpy(p, &r, kPtr);
        }
    } else {
        std::memcpy(p, &r, kPtr);
    }
}

static int readSlotIntish(const Slots* s, const FieldInfo* fi) {
    if (!s) {
        return 0;
    }
    switch (fieldDesc0(fi)) {
    case 'Z':
        return s->getNums() != 0;
    case 'B':
        return static_cast<int>(static_cast<int8_t>(s->getNums()));
    case 'S':
        return static_cast<int>(static_cast<int16_t>(s->getNums() & 0xFFFF));
    case 'C':
        return s->getNums() & 0xFFFF;
    case 'I':
    default:
        return s->getNums();
    }
}

static void writeSlotIntish(Slots* s, const FieldInfo* fi, int v) {
    if (!s) {
        return;
    }
    switch (fieldDesc0(fi)) {
    case 'Z':
        s->setNums(v != 0 ? 1 : 0);
        break;
    case 'B':
        s->setNums(static_cast<int>(static_cast<int8_t>(static_cast<uint8_t>(v & 0xFFU))));
        break;
    case 'S':
        s->setNums(static_cast<int>(static_cast<int16_t>(v & 0xFFFF)));
        break;
    case 'C':
        s->setNums(v & 0xFFFF);
        break;
    case 'I':
    default:
        s->setNums(v);
        break;
    }
}

static long readSlotLong(const Slots* s, const FieldInfo* fi) {
    if (!s) {
        return 0;
    }
    if (fi && fi->getDescriptor() == "J") {
        return s->getLongValue();
    }
    return static_cast<long>(s->getNums());
}

static void writeSlotLong(Slots* s, const FieldInfo* fi, long v) {
    if (!s) {
        return;
    }
    if (fi && fi->getDescriptor() == "J") {
        s->setLongValue(v);
    } else {
        s->setNums(static_cast<int>(v));
    }
}

static float readSlotFloat(const Slots* s, const FieldInfo* fi) {
    if (!s) {
        return 0.f;
    }
    if (fi && fi->getDescriptor() == "F") {
        return s->getFloatValue();
    }
    return 0.f;
}

static void writeSlotFloat(Slots* s, const FieldInfo* fi, float v) {
    if (!s) {
        return;
    }
    if (fi && fi->getDescriptor() == "F") {
        s->setFloatValue(v);
    } else {
        int bits = 0;
        std::memcpy(&bits, &v, sizeof(bits));
        s->setNums(bits);
    }
}

static double readSlotDouble(const Slots* s, const FieldInfo* fi) {
    if (!s) {
        return 0.0;
    }
    if (fi && fi->getDescriptor() == "D") {
        return s->getDoubleValue();
    }
    return 0.0;
}

static void writeSlotDouble(Slots* s, const FieldInfo* fi, double v) {
    if (!s) {
        return;
    }
    if (fi && fi->getDescriptor() == "D") {
        s->setDoubleValue(v);
    }
}

static Object* readSlotRef(const Slots* s, const FieldInfo* fi) {
    if (!s) {
        return nullptr;
    }
    const char t = fieldDesc0(fi);
    if (t == 'L' || t == '[') {
        return s->getRefs();
    }
    return nullptr;
}

static void writeSlotRef(Slots* s, const FieldInfo* fi, Object* r) {
    if (!s) {
        return;
    }
    const char t = fieldDesc0(fi);
    if (t == 'L' || t == '[') {
        s->setRefs(r);
    }
}

} // namespace

struct ReflectFieldSlotCache {
    int clazzSlot{-1};
    int nameSlot{-1};
    bool ready{false};
};

static ReflectFieldSlotCache& reflectFieldSlotCache() {
    static ReflectFieldSlotCache c;
    return c;
}

static void ensureReflectFieldSlots(ClassLoader* cl) {
    ReflectFieldSlotCache& c = reflectFieldSlotCache();
    if (c.ready || !cl) {
        return;
    }
    JavaClass* fc = cl->loadClass("java/lang/reflect/Field");
    if (!fc) {
        c.ready = true;
        return;
    }
    for (FieldInfo* fi : fc->getFieldInfoList()) {
        if (fi->isStatic()) {
            continue;
        }
        if (fi->getName() == "clazz") {
            c.clazzSlot = fi->getSlotId();
        } else if (fi->getName() == "name") {
            c.nameSlot = fi->getSlotId();
        }
    }
    c.ready = true;
}

namespace {

template <bool Volatile>
void unsafeGetIntBody(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int))) {
        st->pushInt(loadRawInt<Volatile>(p));
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushInt(readSlotIntish(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushInt(readSlotIntish(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    st->pushInt(0);
}

template <bool Volatile>
void unsafePutIntBody(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int v = lvt->getInt(4);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int))) {
        storeRawInt<Volatile>(p, v);
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotIntish(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotIntish(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
    }
}

template <bool Volatile>
void unsafeGetLongBody(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(long))) {
        st->pushLong(loadRawLong<Volatile>(p));
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushLong(readSlotLong(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushLong(readSlotLong(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    st->pushLong(0);
}

template <bool Volatile>
void unsafePutLongBody(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const long v = lvt->getLong(4);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(long))) {
        storeRawLong<Volatile>(p, v);
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotLong(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotLong(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
    }
}

template <bool Volatile>
void unsafeGetFloatBody(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(float))) {
        st->pushFloat(loadRawFloat<Volatile>(p));
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushFloat(readSlotFloat(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushFloat(readSlotFloat(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    st->pushFloat(0.f);
}

template <bool Volatile>
void unsafePutFloatBody(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const float v = lvt->getFloat(4);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(float))) {
        storeRawFloat<Volatile>(p, v);
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotFloat(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotFloat(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
    }
}

template <bool Volatile>
void unsafeGetDoubleBody(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(double))) {
        st->pushDouble(loadRawDouble<Volatile>(p));
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushDouble(readSlotDouble(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushDouble(readSlotDouble(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    st->pushDouble(0.0);
}

template <bool Volatile>
void unsafePutDoubleBody(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const double v = lvt->getDouble(4);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(double))) {
        storeRawDouble<Volatile>(p, v);
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotDouble(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotDouble(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
    }
}

template <bool Volatile>
void unsafeGetReferenceBody(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    constexpr size_t kPtr = sizeof(Object*);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, kPtr)) {
        st->pushRef(loadRawRef<Volatile>(p));
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushRef(readSlotRef(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        st->pushRef(readSlotRef(sl, fi));
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_acquire);
        }
        return;
    }
    st->pushRef(nullptr);
}

template <bool Volatile>
void unsafePutReferenceBody(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    Object* v = lvt->getRef(4);
    constexpr size_t kPtr = sizeof(Object*);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, kPtr)) {
        storeRawRef<Volatile>(p, v);
        return;
    }
    if (Slots* sl = unsafeResolveStaticVarSlot(obj, off)) {
        JavaClass* jc = javaClassFromClassMirrorObject(obj);
        const FieldInfo* fi = jc ? findStaticFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotRef(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        return;
    }
    if (Slots* sl = unsafeResolveInstanceSlot(obj, off)) {
        JavaClass* jc = obj->getJavaClass();
        const FieldInfo* fi = jc ? findInstanceFieldBySlot(jc, static_cast<int>(off)) : nullptr;
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
        writeSlotRef(sl, fi, v);
        if constexpr (Volatile) {
            std::atomic_thread_fence(std::memory_order_release);
        }
    }
}

} // namespace

void native_Unsafe_registerNatives(const NativeCallContext& ctx) {
    (void)ctx;
}

void native_Unsafe_getInt(const NativeCallContext& ctx) {
    unsafeGetIntBody<false>(ctx);
}
void native_Unsafe_getIntVolatile(const NativeCallContext& ctx) {
    unsafeGetIntBody<true>(ctx);
}
void native_Unsafe_putInt(const NativeCallContext& ctx) {
    unsafePutIntBody<false>(ctx);
}
void native_Unsafe_putIntVolatile(const NativeCallContext& ctx) {
    unsafePutIntBody<true>(ctx);
}

void native_Unsafe_getLong(const NativeCallContext& ctx) {
    unsafeGetLongBody<false>(ctx);
}
void native_Unsafe_getLongVolatile(const NativeCallContext& ctx) {
    unsafeGetLongBody<true>(ctx);
}
void native_Unsafe_putLong(const NativeCallContext& ctx) {
    unsafePutLongBody<false>(ctx);
}
void native_Unsafe_putLongVolatile(const NativeCallContext& ctx) {
    unsafePutLongBody<true>(ctx);
}

void native_Unsafe_getFloat(const NativeCallContext& ctx) {
    unsafeGetFloatBody<false>(ctx);
}
void native_Unsafe_getFloatVolatile(const NativeCallContext& ctx) {
    unsafeGetFloatBody<true>(ctx);
}
void native_Unsafe_putFloat(const NativeCallContext& ctx) {
    unsafePutFloatBody<false>(ctx);
}
void native_Unsafe_putFloatVolatile(const NativeCallContext& ctx) {
    unsafePutFloatBody<true>(ctx);
}

void native_Unsafe_getDouble(const NativeCallContext& ctx) {
    unsafeGetDoubleBody<false>(ctx);
}
void native_Unsafe_getDoubleVolatile(const NativeCallContext& ctx) {
    unsafeGetDoubleBody<true>(ctx);
}
void native_Unsafe_putDouble(const NativeCallContext& ctx) {
    unsafePutDoubleBody<false>(ctx);
}
void native_Unsafe_putDoubleVolatile(const NativeCallContext& ctx) {
    unsafePutDoubleBody<true>(ctx);
}

void native_Unsafe_getReference(const NativeCallContext& ctx) {
    unsafeGetReferenceBody<false>(ctx);
}
void native_Unsafe_getReferenceVolatile(const NativeCallContext& ctx) {
    unsafeGetReferenceBody<true>(ctx);
}
void native_Unsafe_putReference(const NativeCallContext& ctx) {
    unsafePutReferenceBody<false>(ctx);
}
void native_Unsafe_putReferenceVolatile(const NativeCallContext& ctx) {
    unsafePutReferenceBody<true>(ctx);
}

void native_Unsafe_getBoolean(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        st->pushBoolean(*p != 0);
        return;
    }
    unsafeGetIntBody<false>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushBoolean(v != 0);
}
void native_Unsafe_getBooleanVolatile(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        std::atomic_thread_fence(std::memory_order_acquire);
        st->pushBoolean(*p != 0);
        return;
    }
    unsafeGetIntBody<true>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushBoolean(v != 0);
}
void native_Unsafe_putBoolean(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const unsigned char ub = lvt->getInt(4) != 0 ? 1U : 0U;
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        *p = ub;
        return;
    }
    lvt->setInt(4, ub != 0 ? 1 : 0);
    unsafePutIntBody<false>(ctx);
}
void native_Unsafe_putBooleanVolatile(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const unsigned char ub = lvt->getInt(4) != 0 ? 1U : 0U;
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        std::atomic_thread_fence(std::memory_order_release);
        *p = ub;
        std::atomic_thread_fence(std::memory_order_release);
        return;
    }
    lvt->setInt(4, ub != 0 ? 1 : 0);
    unsafePutIntBody<true>(ctx);
}

void native_Unsafe_getByte(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        st->pushInt(static_cast<int>(static_cast<int8_t>(*p)));
        return;
    }
    unsafeGetIntBody<false>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(static_cast<int8_t>(v)));
}
void native_Unsafe_getByteVolatile(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        std::atomic_thread_fence(std::memory_order_acquire);
        st->pushInt(static_cast<int>(static_cast<int8_t>(*p)));
        return;
    }
    unsafeGetIntBody<true>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(static_cast<int8_t>(v)));
}
void native_Unsafe_putByte(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int raw = lvt->getInt(4);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        *p = static_cast<unsigned char>(static_cast<uint8_t>(raw & 0xFFU));
        return;
    }
    lvt->setInt(4, static_cast<int>(static_cast<int8_t>(static_cast<uint8_t>(raw & 0xFFU))));
    unsafePutIntBody<false>(ctx);
}
void native_Unsafe_putByteVolatile(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int raw = lvt->getInt(4);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, 1)) {
        std::atomic_thread_fence(std::memory_order_release);
        *p = static_cast<unsigned char>(static_cast<uint8_t>(raw & 0xFFU));
        std::atomic_thread_fence(std::memory_order_release);
        return;
    }
    lvt->setInt(4, static_cast<int>(static_cast<int8_t>(static_cast<uint8_t>(raw & 0xFFU))));
    unsafePutIntBody<true>(ctx);
}

void native_Unsafe_getShort(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int16_t))) {
        int16_t v = 0;
        std::memcpy(&v, p, sizeof(v));
        st->pushInt(static_cast<int>(v));
        return;
    }
    unsafeGetIntBody<false>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(static_cast<int16_t>(v & 0xFFFF)));
}
void native_Unsafe_getShortVolatile(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int16_t))) {
        std::atomic_thread_fence(std::memory_order_acquire);
        int16_t v = 0;
        std::memcpy(&v, p, sizeof(v));
        st->pushInt(static_cast<int>(v));
        return;
    }
    unsafeGetIntBody<true>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(static_cast<int16_t>(v & 0xFFFF)));
}
void native_Unsafe_putShort(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int raw = lvt->getInt(4);
    const int16_t v = static_cast<int16_t>(raw & 0xFFFF);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int16_t))) {
        std::memcpy(p, &v, sizeof(v));
        return;
    }
    lvt->setInt(4, static_cast<int>(v));
    unsafePutIntBody<false>(ctx);
}
void native_Unsafe_putShortVolatile(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int raw = lvt->getInt(4);
    const int16_t v = static_cast<int16_t>(raw & 0xFFFF);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int16_t))) {
        std::atomic_thread_fence(std::memory_order_release);
        std::memcpy(p, &v, sizeof(v));
        std::atomic_thread_fence(std::memory_order_release);
        return;
    }
    lvt->setInt(4, static_cast<int>(v));
    unsafePutIntBody<true>(ctx);
}

void native_Unsafe_getChar(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(uint16_t))) {
        uint16_t v = 0;
        std::memcpy(&v, p, sizeof(v));
        st->pushInt(static_cast<int>(v));
        return;
    }
    unsafeGetIntBody<false>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushInt(v & 0xFFFF);
}
void native_Unsafe_getCharVolatile(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(uint16_t))) {
        std::atomic_thread_fence(std::memory_order_acquire);
        uint16_t v = 0;
        std::memcpy(&v, p, sizeof(v));
        st->pushInt(static_cast<int>(v));
        return;
    }
    unsafeGetIntBody<true>(ctx);
    const int v = ctx.caller->getOperandStack()->popInt();
    ctx.caller->getOperandStack()->pushInt(v & 0xFFFF);
}
void native_Unsafe_putChar(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const uint16_t v = static_cast<uint16_t>(lvt->getInt(4) & 0xFFFF);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(uint16_t))) {
        std::memcpy(p, &v, sizeof(v));
        return;
    }
    lvt->setInt(4, static_cast<int>(v));
    unsafePutIntBody<false>(ctx);
}
void native_Unsafe_putCharVolatile(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const uint16_t v = static_cast<uint16_t>(lvt->getInt(4) & 0xFFFF);
    if (unsigned char* p = unsafeResolveArrayOrAbsolute(obj, off, sizeof(uint16_t))) {
        std::atomic_thread_fence(std::memory_order_release);
        std::memcpy(p, &v, sizeof(v));
        std::atomic_thread_fence(std::memory_order_release);
        return;
    }
    lvt->setInt(4, static_cast<int>(v));
    unsafePutIntBody<true>(ctx);
}

void native_Unsafe_allocateMemory0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    // 实例方法 slot0=this；long 占两槽时首槽下标为 1（与 JVM 局部变量布局一致）。
    const long sz = lvt->getLong(1);
    if (sz <= 0) {
        ctx.caller->getOperandStack()->pushLong(0);
        return;
    }
    void* p = std::malloc(static_cast<size_t>(sz));
    ctx.caller->getOperandStack()->pushLong(reinterpret_cast<long>(p));
}

void native_Unsafe_reallocateMemory0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    void* oldPtr = reinterpret_cast<void*>(lvt->getLong(1));
    const long sz = lvt->getLong(3);
    if (sz <= 0) {
        if (oldPtr) {
            std::free(oldPtr);
        }
        ctx.caller->getOperandStack()->pushLong(0);
        return;
    }
    void* p = std::realloc(oldPtr, static_cast<size_t>(sz));
    ctx.caller->getOperandStack()->pushLong(reinterpret_cast<long>(p));
}

void native_Unsafe_freeMemory0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    void* p = reinterpret_cast<void*>(lvt->getLong(1));
    if (p) {
        std::free(p);
    }
}

void native_Unsafe_setMemory0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long offset = lvt->getLong(2);
    const long size = lvt->getLong(4);
    const int b = lvt->getInt(6);
    if (size <= 0) {
        return;
    }
    const auto ub = static_cast<unsigned char>(b & 255);
    if (!obj) {
        std::memset(reinterpret_cast<void*>(static_cast<uintptr_t>(offset)), static_cast<int>(ub), static_cast<size_t>(size));
        return;
    }
    if (obj->getArrayLength() < 0) {
        return;
    }
    unsigned char* p = unsafeResolveArrayOrAbsolute(obj, offset, static_cast<size_t>(size));
    if (!p) {
        return;
    }
    std::memset(p, static_cast<int>(ub), static_cast<size_t>(size));
}

void native_Unsafe_copyMemory0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* srcObj = lvt->getRef(1);
    const long srcOff = lvt->getLong(2);
    Object* dstObj = lvt->getRef(4);
    const long dstOff = lvt->getLong(5);
    const long nbytes = lvt->getLong(7);
    if (nbytes <= 0) {
        return;
    }
    unsigned char* srcPtr = nullptr;
    unsigned char* dstPtr = nullptr;
    if (!srcObj) {
        srcPtr = reinterpret_cast<unsigned char*>(static_cast<uintptr_t>(srcOff));
    } else {
        if (srcObj->getArrayLength() < 0) {
            return;
        }
        srcPtr = unsafeResolveArrayOrAbsolute(srcObj, srcOff, static_cast<size_t>(nbytes));
        if (!srcPtr) {
            return;
        }
    }
    if (!dstObj) {
        dstPtr = reinterpret_cast<unsigned char*>(static_cast<uintptr_t>(dstOff));
    } else {
        if (dstObj->getArrayLength() < 0) {
            return;
        }
        dstPtr = unsafeResolveArrayOrAbsolute(dstObj, dstOff, static_cast<size_t>(nbytes));
        if (!dstPtr) {
            return;
        }
    }
    std::memmove(dstPtr, srcPtr, static_cast<size_t>(nbytes));
}

void native_Unsafe_fullFence(const NativeCallContext& ctx) {
    (void)ctx;
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

void native_Unsafe_loadFence(const NativeCallContext& ctx) {
    (void)ctx;
    std::atomic_thread_fence(std::memory_order_acquire);
}

void native_Unsafe_storeFence(const NativeCallContext& ctx) {
    (void)ctx;
    std::atomic_thread_fence(std::memory_order_release);
}

void native_Unsafe_compareAndSetInt(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int expected = lvt->getInt(4);
    const int desired = lvt->getInt(5);
    OperandStack* st = ctx.caller->getOperandStack();

    if (unsigned char* win = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int))) {
        if (isAlignedForAtomic<int>(win)) {
            auto* a = reinterpret_cast<std::atomic<int>*>(win);
            int e = expected;
            const bool ok = a->compare_exchange_strong(e, desired, std::memory_order_acq_rel, std::memory_order_acquire);
            st->pushBoolean(ok);
            return;
        }
        int cur = 0;
        std::memcpy(&cur, win, sizeof(int));
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        std::memcpy(win, &desired, sizeof(int));
        st->pushBoolean(true);
        return;
    }
    if (Slots* stSlot = unsafeResolveStaticVarSlot(obj, off)) {
        const int cur = stSlot->getNums();
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        stSlot->setNums(desired);
        st->pushBoolean(true);
        return;
    }
    if (Slots* slot = unsafeResolveInstanceSlot(obj, off)) {
        const int cur = slot->getNums();
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        slot->setNums(desired);
        st->pushBoolean(true);
        return;
    }
    st->pushBoolean(false);
}

void native_Unsafe_compareAndExchangeInt(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const int expected = lvt->getInt(4);
    const int desired = lvt->getInt(5);
    OperandStack* st = ctx.caller->getOperandStack();

    if (unsigned char* win = unsafeResolveArrayOrAbsolute(obj, off, sizeof(int))) {
        if (isAlignedForAtomic<int>(win)) {
            auto* a = reinterpret_cast<std::atomic<int>*>(win);
            int witness = expected;
            (void)a->compare_exchange_strong(witness, desired, std::memory_order_acq_rel, std::memory_order_acquire);
            st->pushInt(witness);
            return;
        }
        int cur = 0;
        std::memcpy(&cur, win, sizeof(int));
        if (cur == expected) {
            std::memcpy(win, &desired, sizeof(int));
        }
        st->pushInt(cur);
        return;
    }
    if (Slots* stSlot = unsafeResolveStaticVarSlot(obj, off)) {
        const int cur = stSlot->getNums();
        if (cur == expected) {
            stSlot->setNums(desired);
        }
        st->pushInt(cur);
        return;
    }
    if (Slots* slot = unsafeResolveInstanceSlot(obj, off)) {
        const int cur = slot->getNums();
        if (cur == expected) {
            slot->setNums(desired);
        }
        st->pushInt(cur);
        return;
    }
    st->pushInt(0);
}

void native_Unsafe_compareAndSetLong(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const long expected = lvt->getLong(4);
    const long desired = lvt->getLong(6);
    OperandStack* st = ctx.caller->getOperandStack();

    if (unsigned char* win = unsafeResolveArrayOrAbsolute(obj, off, sizeof(long))) {
        if (isAlignedForAtomic<int64_t>(win)) {
            auto* a = reinterpret_cast<std::atomic<int64_t>*>(win);
            int64_t e = static_cast<int64_t>(expected);
            const int64_t d = static_cast<int64_t>(desired);
            const bool ok =
                a->compare_exchange_strong(e, d, std::memory_order_acq_rel, std::memory_order_acquire);
            st->pushBoolean(ok);
            return;
        }
        long cur = 0;
        std::memcpy(&cur, win, sizeof(long));
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        std::memcpy(win, &desired, sizeof(long));
        st->pushBoolean(true);
        return;
    }
    if (Slots* stSlot = unsafeResolveStaticVarSlot(obj, off)) {
        const long cur = stSlot->getLongValue();
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        stSlot->setLongValue(desired);
        st->pushBoolean(true);
        return;
    }
    if (Slots* slot = unsafeResolveInstanceSlot(obj, off)) {
        const long cur = slot->getLongValue();
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        slot->setLongValue(desired);
        st->pushBoolean(true);
        return;
    }
    st->pushBoolean(false);
}

void native_Unsafe_compareAndExchangeLong(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    const long expected = lvt->getLong(4);
    const long desired = lvt->getLong(6);
    OperandStack* st = ctx.caller->getOperandStack();

    if (unsigned char* win = unsafeResolveArrayOrAbsolute(obj, off, sizeof(long))) {
        if (isAlignedForAtomic<int64_t>(win)) {
            auto* a = reinterpret_cast<std::atomic<int64_t>*>(win);
            int64_t witness = static_cast<int64_t>(expected);
            const int64_t d = static_cast<int64_t>(desired);
            (void)a->compare_exchange_strong(witness, d, std::memory_order_acq_rel, std::memory_order_acquire);
            st->pushLong(static_cast<long>(witness));
            return;
        }
        long cur = 0;
        std::memcpy(&cur, win, sizeof(long));
        if (cur == expected) {
            std::memcpy(win, &desired, sizeof(long));
        }
        st->pushLong(cur);
        return;
    }
    if (Slots* stSlot = unsafeResolveStaticVarSlot(obj, off)) {
        const long cur = stSlot->getLongValue();
        if (cur == expected) {
            stSlot->setLongValue(desired);
        }
        st->pushLong(cur);
        return;
    }
    if (Slots* slot = unsafeResolveInstanceSlot(obj, off)) {
        const long cur = slot->getLongValue();
        if (cur == expected) {
            slot->setLongValue(desired);
        }
        st->pushLong(cur);
        return;
    }
    st->pushLong(0);
}

void native_Unsafe_compareAndSetReference(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    Object* expected = lvt->getRef(4);
    Object* desired = lvt->getRef(5);
    OperandStack* st = ctx.caller->getOperandStack();
    constexpr size_t kPtr = sizeof(Object*);

    if (unsigned char* win = unsafeResolveArrayOrAbsolute(obj, off, kPtr)) {
        if (isAlignedForAtomic<uintptr_t>(win)) {
            auto* a = reinterpret_cast<std::atomic<uintptr_t>*>(win);
            uintptr_t e = reinterpret_cast<uintptr_t>(expected);
            const uintptr_t d = reinterpret_cast<uintptr_t>(desired);
            const bool ok =
                a->compare_exchange_strong(e, d, std::memory_order_acq_rel, std::memory_order_acquire);
            st->pushBoolean(ok);
            return;
        }
        Object* cur = nullptr;
        std::memcpy(&cur, win, kPtr);
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        std::memcpy(win, &desired, kPtr);
        st->pushBoolean(true);
        return;
    }
    if (Slots* stSlot = unsafeResolveStaticVarSlot(obj, off)) {
        Object* cur = stSlot->getRefs();
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        stSlot->setRefs(desired);
        st->pushBoolean(true);
        return;
    }
    if (Slots* slot = unsafeResolveInstanceSlot(obj, off)) {
        Object* cur = slot->getRefs();
        if (cur != expected) {
            st->pushBoolean(false);
            return;
        }
        slot->setRefs(desired);
        st->pushBoolean(true);
        return;
    }
    st->pushBoolean(false);
}

void native_Unsafe_compareAndExchangeReference(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* obj = lvt->getRef(1);
    const long off = lvt->getLong(2);
    Object* expected = lvt->getRef(4);
    Object* desired = lvt->getRef(5);
    OperandStack* st = ctx.caller->getOperandStack();
    constexpr size_t kPtr = sizeof(Object*);

    if (unsigned char* win = unsafeResolveArrayOrAbsolute(obj, off, kPtr)) {
        if (isAlignedForAtomic<uintptr_t>(win)) {
            auto* a = reinterpret_cast<std::atomic<uintptr_t>*>(win);
            uintptr_t witness = reinterpret_cast<uintptr_t>(expected);
            const uintptr_t d = reinterpret_cast<uintptr_t>(desired);
            (void)a->compare_exchange_strong(witness, d, std::memory_order_acq_rel, std::memory_order_acquire);
            st->pushRef(reinterpret_cast<Object*>(witness));
            return;
        }
        Object* cur = nullptr;
        std::memcpy(&cur, win, kPtr);
        if (cur == expected) {
            std::memcpy(win, &desired, kPtr);
        }
        st->pushRef(cur);
        return;
    }
    if (Slots* stSlot = unsafeResolveStaticVarSlot(obj, off)) {
        Object* cur = stSlot->getRefs();
        if (cur == expected) {
            stSlot->setRefs(desired);
        }
        st->pushRef(cur);
        return;
    }
    if (Slots* slot = unsafeResolveInstanceSlot(obj, off)) {
        Object* cur = slot->getRefs();
        if (cur == expected) {
            slot->setRefs(desired);
        }
        st->pushRef(cur);
        return;
    }
    st->pushRef(nullptr);
}

void native_Unsafe_arrayBaseOffset0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(1);
    JavaClass* jc = javaClassFromClassMirrorObject(mirror);
    if (!jc || !jc->isArray()) {
        ctx.caller->getOperandStack()->pushInt(0);
        return;
    }
    ctx.caller->getOperandStack()->pushInt(ArrayCoalloc::baseOffsetBytes());
}

void native_Unsafe_arrayIndexScale0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(1);
    JavaClass* jc = javaClassFromClassMirrorObject(mirror);
    OperandStack* st = ctx.caller->getOperandStack();
    if (!jc || !jc->isArray()) {
        st->pushInt(0);
        return;
    }
    const std::string& n = jc->getThisClassName();
    const size_t esz = unsafePrimArrayElemBytes(n);
    if (esz > 0) {
        st->pushInt(static_cast<int>(esz));
        return;
    }
    if (n.size() >= 2 && n[0] == '[' && (n[1] == 'L' || n[1] == '[')) {
        st->pushInt(static_cast<int>(sizeof(Object*)));
        return;
    }
    st->pushInt(0);
}

void native_Unsafe_objectFieldOffset1(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(1);
    Object* nameStr = ctx.callee->getLocalVariableTables()->getRef(2);
    OperandStack* st = ctx.caller->getOperandStack();
    JavaClass* decl = javaClassFromClassMirrorObject(mirror);
    if (!decl || !nameStr) {
        st->pushLong(-1);
        return;
    }
    const std::string fname = Heap::JString::getJString()->getJavaString(nameStr);
    FieldInfo* fi = findInstanceFieldByName(decl, fname);
    if (!fi) {
        st->pushLong(-1);
        return;
    }
    st->pushLong(static_cast<long>(fi->getSlotId()));
}

void native_Unsafe_objectFieldOffset0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCalleeUnsafe(ctx);
    ensureReflectFieldSlots(cl);
    ReflectFieldSlotCache& rc = reflectFieldSlotCache();
    Object* fobj = ctx.callee->getLocalVariableTables()->getRef(1);
    OperandStack* st = ctx.caller->getOperandStack();
    if (!fobj || rc.clazzSlot < 0 || rc.nameSlot < 0) {
        st->pushLong(-1);
        return;
    }
    Slots* ff = fobj->getFields();
    if (!ff) {
        st->pushLong(-1);
        return;
    }
    Object* dm = ff[rc.clazzSlot].getRefs();
    Object* nms = ff[rc.nameSlot].getRefs();
    JavaClass* decl = javaClassFromClassMirrorObject(dm);
    if (!decl || !nms) {
        st->pushLong(-1);
        return;
    }
    const std::string fname = Heap::JString::getJString()->getJavaString(nms);
    FieldInfo* fi = findInstanceFieldByName(decl, fname);
    if (!fi) {
        st->pushLong(-1);
        return;
    }
    st->pushLong(static_cast<long>(fi->getSlotId()));
}

void native_Unsafe_staticFieldOffset0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCalleeUnsafe(ctx);
    ensureReflectFieldSlots(cl);
    ReflectFieldSlotCache& rc = reflectFieldSlotCache();
    Object* fobj = ctx.callee->getLocalVariableTables()->getRef(1);
    OperandStack* st = ctx.caller->getOperandStack();
    if (!fobj || rc.clazzSlot < 0 || rc.nameSlot < 0) {
        st->pushLong(-1);
        return;
    }
    Slots* ff = fobj->getFields();
    if (!ff) {
        st->pushLong(-1);
        return;
    }
    Object* dm = ff[rc.clazzSlot].getRefs();
    Object* nms = ff[rc.nameSlot].getRefs();
    JavaClass* decl = javaClassFromClassMirrorObject(dm);
    if (!decl || !nms) {
        st->pushLong(-1);
        return;
    }
    const std::string fname = Heap::JString::getJString()->getJavaString(nms);
    FieldInfo* fi = findStaticFieldByName(decl, fname);
    if (!fi) {
        st->pushLong(-1);
        return;
    }
    st->pushLong(static_cast<long>(fi->getSlotId()));
}

void native_Unsafe_staticFieldBase0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCalleeUnsafe(ctx);
    ensureReflectFieldSlots(cl);
    ReflectFieldSlotCache& rc = reflectFieldSlotCache();
    Object* fobj = ctx.callee->getLocalVariableTables()->getRef(1);
    OperandStack* st = ctx.caller->getOperandStack();
    if (!fobj || !cl || rc.clazzSlot < 0) {
        st->pushRef(nullptr);
        return;
    }
    Slots* ff = fobj->getFields();
    if (!ff) {
        st->pushRef(nullptr);
        return;
    }
    Object* dm = ff[rc.clazzSlot].getRefs();
    JavaClass* decl = javaClassFromClassMirrorObject(dm);
    if (!decl) {
        st->pushRef(nullptr);
        return;
    }
    st->pushRef(mirrorForJavaClass(decl, cl));
}

void native_Unsafe_shouldBeInitialized0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(1);
    OperandStack* st = ctx.caller->getOperandStack();
    JavaClass* jc = javaClassFromClassMirrorObject(mirror);
    if (!jc) {
        st->pushBoolean(true);
        return;
    }
    if (jc->getClinitMethod() == nullptr) {
        st->pushBoolean(true);
        return;
    }
    st->pushBoolean(jc->initCompleted());
}

void native_Unsafe_ensureClassInitialized0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(1);
    JavaClass* jc = javaClassFromClassMirrorObject(mirror);
    if (!jc) {
        return;
    }
    JavaThread* th = ctx.callee->getJavaThread();
    if (!th) {
        return;
    }
    if (!jc->initCompleted() && !jc->initStarted()) {
        jc->initJavaClass(th, jc);
    }
}

void native_Unsafe_copySwapMemory0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* srcObj = lvt->getRef(1);
    const long srcOff = lvt->getLong(2);
    Object* dstObj = lvt->getRef(4);
    const long dstOff = lvt->getLong(5);
    const long nbytes = lvt->getLong(7);
    const long elem = lvt->getLong(9);
    if (nbytes <= 0 || elem <= 0) {
        return;
    }
    const size_t nb = static_cast<size_t>(nbytes);
    const size_t esz = static_cast<size_t>(elem);
    if (esz == 0 || nb % esz != 0) {
        return;
    }
    unsigned char* s = unsafeResolveArrayOrAbsolute(srcObj, srcOff, nb);
    unsigned char* d = unsafeResolveArrayOrAbsolute(dstObj, dstOff, nb);
    if (!s || !d) {
        return;
    }
    copySwapMemorySpan(d, s, nb, esz);
}

void native_Unsafe_writeback0(const NativeCallContext& ctx) {
    (void)ctx;
}

void native_Unsafe_writebackPreSync0(const NativeCallContext& ctx) {
    (void)ctx;
}

void native_Unsafe_writebackPostSync0(const NativeCallContext& ctx) {
    (void)ctx;
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

void native_Unsafe_getLoadAverage0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* arr = lvt->getRef(1);
    const int nelem = lvt->getInt(2);
    OperandStack* st = ctx.caller->getOperandStack();
    if (!arr || nelem <= 0) {
        st->pushInt(-1);
        return;
    }
    JavaClass* jc = arr->getJavaClass();
    if (!jc || jc->getThisClassName() != "[D" || arr->getArrayLength() < 0) {
        st->pushInt(-1);
        return;
    }
    const int alen = arr->getArrayLength();
    const int n = (std::min)(nelem, alen);
    auto* buf = reinterpret_cast<double*>(arr->getData());
    if (!buf && n > 0) {
        st->pushInt(-1);
        return;
    }
    for (int i = 0; i < n; ++i) {
        buf[static_cast<size_t>(i)] = 0.0;
    }
    st->pushInt(n);
}

namespace {

thread_local Object* tlsUnsafeParkSelfOop = nullptr;

struct UnsafeParkHub {
    std::mutex mu;
    std::condition_variable cv;
    int permits{0};
};

std::mutex gUnsafeParkHubsMapMu;
std::unordered_map<Object*, std::shared_ptr<UnsafeParkHub>> gUnsafeParkHubs;

std::shared_ptr<UnsafeParkHub> parkHubFor(Object* t) {
    if (!t) {
        return nullptr;
    }
    std::lock_guard<std::mutex> outer(gUnsafeParkHubsMapMu);
    std::shared_ptr<UnsafeParkHub>& slot = gUnsafeParkHubs[t];
    if (!slot) {
        slot = std::make_shared<UnsafeParkHub>();
    }
    return slot;
}

} // namespace

void unsafeParkBindCurrentThreadOop(Object* threadOop) {
    tlsUnsafeParkSelfOop = threadOop;
}

void native_Unsafe_park(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    const bool isAbsolute = lvt->getInt(1) != 0;
    const long timeArg = lvt->getLong(2);

    Object* self = tlsUnsafeParkSelfOop;
    if (!self) {
        return;
    }
    std::shared_ptr<UnsafeParkHub> hub = parkHubFor(self);
    if (!hub) {
        return;
    }

    std::unique_lock<std::mutex> lk(hub->mu);
    const auto hasPermit = [&] { return hub->permits > 0; };
    if (hasPermit()) {
        --hub->permits;
        return;
    }

    if (isAbsolute) {
        if (timeArg <= 0) {
            while (!hasPermit()) {
                hub->cv.wait(lk);
            }
        } else {
            const std::chrono::system_clock::time_point deadline{
                std::chrono::milliseconds{timeArg}};
            while (!hasPermit()) {
                if (hub->cv.wait_until(lk, deadline) == std::cv_status::timeout) {
                    break;
                }
            }
        }
    } else {
        if (timeArg <= 0) {
            while (!hasPermit()) {
                hub->cv.wait(lk);
            }
        } else {
            const auto deadline =
                std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::nanoseconds(timeArg));
            while (!hasPermit()) {
                if (hub->cv.wait_until(lk, deadline) == std::cv_status::timeout) {
                    break;
                }
            }
        }
    }

    if (hasPermit()) {
        --hub->permits;
    }
}

void native_Unsafe_unpark(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    Object* t = ctx.callee->getLocalVariableTables()->getRef(1);
    if (!t) {
        return;
    }
    std::shared_ptr<UnsafeParkHub> hub = parkHubFor(t);
    {
        std::lock_guard<std::mutex> lk(hub->mu);
        ++hub->permits;
    }
    hub->cv.notify_one();
}

void native_Unsafe_allocateInstance(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* mirror = ctx.callee->getLocalVariableTables()->getRef(1);
    JavaThread* th = ctx.callee->getJavaThread();
    OperandStack* st = ctx.caller->getOperandStack();
    if (!mirror) {
        throwNullPointerException(ctx.caller);
    }
    JavaClass* jc = javaClassFromClassMirrorObject(mirror);
    if (!th) {
        st->pushRef(nullptr);
        return;
    }
    if (!jc) {
        throwInstantiationException(ctx.caller);
    }
    const std::string& n = jc->getThisClassName();
    if (jc->isArray() || jc->isInterface() || jc->isAbstract()) {
        throwInstantiationException(ctx.caller);
    }
    if (n.size() == 1 && std::string("IJZBSFCD").find(n[0]) != std::string::npos) {
        throwInstantiationException(ctx.caller);
    }
    if (!jc->initCompleted() && !jc->initStarted()) {
        jc->initJavaClass(th, jc);
    }
    Object* inst = jc->createNewJavaObjectInstance();
    st->pushRef(inst);
}

void native_Unsafe_throwException(const NativeCallContext& ctx) {
    if (!ctx.callee || !ctx.caller) {
        return;
    }
    Object* ex = ctx.callee->getLocalVariableTables()->getRef(1);
    if (!ex) {
        throwNullPointerException(ctx.caller);
    }
    throw JvmThrownObject(ex);
}

void native_Unsafe_defineClass0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* nameStr = lvt->getRef(1);
    Object* bArr = lvt->getRef(2);
    const int off = lvt->getInt(3);
    const int len = lvt->getInt(4);
    (void)lvt->getRef(5);  // ClassLoader：尚无 Java OOP -> C++ ClassLoader 映射，使用定义方类加载器
    (void)lvt->getRef(6); // ProtectionDomain：当前未使用

    OperandStack* st = ctx.caller->getOperandStack();
    ClassLoader* cl = loaderFromCalleeUnsafe(ctx);
    if (!cl) {
        st->pushRef(nullptr);
        return;
    }
    if (!bArr) {
        throwNullPointerException(ctx.caller);
    }
    JavaClass* arrCls = bArr->getJavaClass();
    if (!arrCls || arrCls->getThisClassName() != "[B") {
        throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: expected byte[]");
    }
    const int alen = bArr->getArrayLength();
    if (alen < 0 || off < 0 || len < 0) {
        throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: invalid offset/length");
    }
    const long long end = static_cast<long long>(off) + static_cast<long long>(len);
    if (end > alen || end > INT_MAX) {
        throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: range out of bounds");
    }
    const auto* raw = reinterpret_cast<const unsigned char*>(bArr->getData());
    if (len < 4 || raw[off + 0] != 0xCA || raw[off + 1] != 0xFE || raw[off + 2] != 0xBA || raw[off + 3] != 0xBE) {
        throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: invalid class file magic");
    }

    std::string expectedInternal;
    if (nameStr) {
        expectedInternal = binaryNameToInternalForDefine(Heap::JString::getJString()->getJavaString(nameStr));
        if (!expectedInternal.empty() && cl->getLoadedClass(expectedInternal) != nullptr) {
            throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: duplicate class definition");
        }
    }

    std::string payload(reinterpret_cast<const char*>(raw + off), static_cast<size_t>(len));
    auto* q = new Lang::ByteQueue();
    q->pushStringChars(payload);

    JavaClass* jc = nullptr;
    try {
        jc = cl->defineClass(q);
    } catch (...) {
        throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: class file parse failed");
    }
    if (!jc) {
        st->pushRef(nullptr);
        return;
    }
    if (!expectedInternal.empty() && jc->getThisClassName() != expectedInternal) {
        throw std::runtime_error("jdk.internal.misc.Unsafe.defineClass0: class name mismatch");
    }
    cl->link(jc);
    Object* mirror = mirrorForJavaClass(jc, cl);
    st->pushRef(mirror);
}

void native_Unsafe_getUncompressedObject(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    const long addrLong = ctx.callee->getLocalVariableTables()->getLong(1);
    OperandStack* st = ctx.caller->getOperandStack();
    if (addrLong == 0) {
        st->pushRef(nullptr);
        return;
    }
    void* cell = reinterpret_cast<void*>(static_cast<uintptr_t>(addrLong));
    Object* ref = nullptr;
    std::memcpy(&ref, cell, sizeof(Object*));
    st->pushRef(ref);
}

void native_Unsafe_invokeCleaner(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* buf = ctx.callee->getLocalVariableTables()->getRef(1);
    if (!buf) {
        throwNullPointerException(ctx.caller);
    }
    JavaClass* jc = buf->getJavaClass();
    if (!jc) {
        return;
    }
    const std::string& n = jc->getThisClassName();
    const bool looksDirect = (n.size() >= 17 && n.compare(0, 17, "java/nio/Direct") == 0);
    if (!looksDirect || n.find("Mapped") != std::string::npos) {
        throw std::runtime_error("jdk.internal.misc.Unsafe.invokeCleaner: buffer is non-direct");
    }
    Slots* fields = buf->getFields();
    if (!fields) {
        return;
    }
    FieldInfo* attF = findInstanceFieldByName(jc, "att");
    if (!attF) {
        attF = findInstanceFieldByName(jc, "attachment");
    }
    if (attF) {
        const int si = attF->getSlotId();
        if (si >= 0 && static_cast<size_t>(si) < static_cast<size_t>(jc->getInstanceCount())) {
            if (fields[si].getRefs()) {
                throw std::runtime_error("jdk.internal.misc.Unsafe.invokeCleaner: duplicate or slice buffer");
            }
        }
    }
    FieldInfo* clnF = findInstanceFieldByName(jc, "cleaner");
    if (!clnF) {
        return;
    }
    const int clSlot = clnF->getSlotId();
    if (clSlot < 0 || static_cast<size_t>(clSlot) >= static_cast<size_t>(jc->getInstanceCount())) {
        return;
    }
    if (!fields[clSlot].getRefs()) {
        return;
    }
    FieldInfo* addrF = findInstanceFieldByName(jc, "address");
    if (!addrF || addrF->getDescriptor() != "J") {
        fields[clSlot].setRefs(nullptr);
        return;
    }
    const int addrSlot = addrF->getSlotId();
    if (addrSlot < 0 || static_cast<size_t>(addrSlot) >= static_cast<size_t>(jc->getInstanceCount())) {
        fields[clSlot].setRefs(nullptr);
        return;
    }
    long addr = fields[addrSlot].getLongValue();
    if (addr != 0) {
        void* p = reinterpret_cast<void*>(static_cast<uintptr_t>(addr));
        std::free(p);
        fields[addrSlot].setLongValue(0);
    }
    fields[clSlot].setRefs(nullptr);
}

} // namespace Native
} // namespace Runtime
