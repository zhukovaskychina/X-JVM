//
// multianewarray
//

#include "MultiANewArray.h"

#include "../../runtime/JavaFrame.h"
#include "../../runtime/Object.h"
#include "../../runtime/OperandStack.h"
#include "../../runtime/heap/ConstantsPoolObject.h"
#include "../../runtime/heap/JavaClass.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace Instruction {

namespace {

int arrayArity(const std::string &name) {
    int n = 0;
    while (n < static_cast<int>(name.size()) && name[static_cast<size_t>(n)] == '[') {
        ++n;
    }
    return n;
}

/** `[[I` -> `[I`；`[I` -> `[I`；`[Ljava/lang/String;` -> `[Ljava/lang/String;`。 */
std::string nestedOneDimArrayDescriptor(const std::string &arrayClassName) {
    if (arrayClassName.size() < 2 || arrayClassName[0] != '[') {
        return "";
    }
    const std::string r = arrayClassName.substr(1);
    if (r.empty()) {
        return "";
    }
    if (r[0] == '[') {
        return r;
    }
    return std::string("[") + r;
}

Runtime::Object *allocPartial(Runtime::JavaClass *cur, const std::vector<int> &dims, std::size_t idx, std::size_t d,
                              Runtime::ClassLoader *cl) {
    if (idx >= d || !cur || !cl) {
        return nullptr;
    }
    const int len = dims[idx];
    if (len < 0) {
        throw std::runtime_error("NegativeArraySizeException");
    }
    Runtime::Object *outer = cur->createArray(len);
    if (!outer || idx == d - 1) {
        return outer;
    }
    const std::string innerDesc = nestedOneDimArrayDescriptor(cur->getThisClassName());
    Runtime::JavaClass *inner = cl->loadClass(innerDesc);
    if (!inner) {
        throw std::runtime_error("multianewarray: load inner array class failed");
    }
    auto **slots = reinterpret_cast<Runtime::Object **>(outer->getData());
    for (int i = 0; i < len; ++i) {
        slots[i] = allocPartial(inner, dims, idx + 1, d, cl);
    }
    return outer;
}

Runtime::Object *allocFull(Runtime::JavaClass *cur, const std::vector<int> &dims, std::size_t idx,
                           Runtime::ClassLoader *cl) {
    if (idx >= dims.size() || !cur || !cl) {
        return nullptr;
    }
    const int len = dims[idx];
    if (len < 0) {
        throw std::runtime_error("NegativeArraySizeException");
    }
    if (idx == dims.size() - 1) {
        return cur->createArray(len);
    }
    const std::string innerDesc = nestedOneDimArrayDescriptor(cur->getThisClassName());
    Runtime::JavaClass *inner = cl->loadClass(innerDesc);
    if (!inner) {
        throw std::runtime_error("multianewarray: load inner array class failed");
    }
    Runtime::Object *outer = cur->createArray(len);
    if (!outer || len == 0) {
        return outer;
    }
    auto **slots = reinterpret_cast<Runtime::Object **>(outer->getData());
    for (int i = 0; i < len; ++i) {
        slots[i] = allocFull(inner, dims, idx + 1, cl);
    }
    return outer;
}

} // namespace

void MultiANewArray::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
    poolIndex_ = byteCodeReader->readU2();
    dimCount_ = byteCodeReader->readU1();
}

void MultiANewArray::execute(Runtime::JavaFrame *javaFrame) {
    const u1 d = dimCount_;
    if (d == 0) {
        throw std::runtime_error("multianewarray: dimensions == 0");
    }
    std::vector<int> counts(static_cast<size_t>(d));
    for (int i = static_cast<int>(d) - 1; i >= 0; --i) {
        counts[static_cast<size_t>(i)] = javaFrame->getOperandStack()->popInt();
    }
    Runtime::JavaClass *cur = javaFrame->getMethod()->getJavaClass();
    Runtime::Heap::RuntimeConstantsPool *cp = cur->getRuntimeConstantsPool();
    if (!cp) {
        throw std::runtime_error("multianewarray: no constant pool");
    }
    Runtime::Heap::ConstantsPoolObject *po = cp->getIndex(poolIndex_);
    if (!po) {
        throw std::runtime_error("multianewarray: bad pool index");
    }
    Runtime::Heap::ClassRef *cref = po->getClassRef();
    if (!cref) {
        throw std::runtime_error("multianewarray: expected CONSTANT_Class");
    }
    Runtime::JavaClass *target = cref->resolvedJavaClass();
    Runtime::ClassLoader *cl = cur->getClassLoader();
    if (!target || !cl) {
        throw std::runtime_error("multianewarray: resolve failed");
    }
    const std::string &tn = target->getThisClassName();
    const int arity = arrayArity(tn);
    if (arity == 0) {
        throw std::runtime_error("multianewarray: not an array class");
    }
    if (static_cast<int>(d) > arity) {
        throw std::runtime_error("multianewarray: dimensions > array arity");
    }
    Runtime::Object *arr = nullptr;
    if (d == static_cast<u1>(arity)) {
        arr = allocFull(target, counts, 0, cl);
    } else {
        arr = allocPartial(target, counts, 0, static_cast<std::size_t>(d), cl);
    }
    javaFrame->getOperandStack()->pushRef(arr);
}

} // namespace Instruction
