#include "PutStatic.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../utils/StringUtils.h"

#include <stdexcept>

namespace Instruction {

void PutStatic::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Heap::Method* currentMethod = javaFrame->getMethod();
    Runtime::JavaClass* currentJavaClass = currentMethod->getJavaClass();
    Runtime::Heap::RuntimeConstantsPool* runtimeConstantsPool = currentJavaClass->getRuntimeConstantsPool();
    Runtime::Heap::FieldRef* fieldRefs = runtimeConstantsPool->getIndex(this->getIndex())->getFieldRef();
    Runtime::Heap::FieldInfo* field = fieldRefs->resolvedFields();
    Runtime::JavaClass* fieldJavaClass = field->getJavaClass();

    if (!fieldJavaClass->initStarted()) {
        javaFrame->revertNextPc();
        fieldJavaClass->initJavaClass(javaFrame->getJavaThread(), fieldJavaClass);
        return;
    }

    if (!field->isStatic()) {
        throw std::runtime_error("PutStatic: non-static field");
    }

    if (field->isFinal()) {
        if (currentJavaClass != fieldJavaClass || currentMethod->getName() != "<clinit>") {
            // 放宽：仍允许写入（与部分教学 JVM 一致）；严格模式可在此抛错。
        }
    }

    const std::string descriptor = field->getDescriptor();
    const u1 slotId = field->getSlotId();
    std::vector<Runtime::Slots*> slotsVector = fieldJavaClass->getStaticVars();
    if (static_cast<size_t>(slotId) >= slotsVector.size()) {
        throw std::runtime_error("PutStatic: bad static slot index");
    }
    Runtime::Slots* slot = slotsVector[slotId];
    if (!slot) {
        slot = new Runtime::Slots();
        slotsVector[slotId] = slot;
    }

    if (descriptor == "Z" || descriptor == "B" || descriptor == "C" || descriptor == "S" || descriptor == "I") {
        const int v = javaFrame->getOperandStack()->popInt();
        slot->setNums(v);
    } else if (descriptor == "F") {
        const float v = javaFrame->getOperandStack()->popFloat();
        slot->setFloatValue(v);
    } else if (descriptor == "J") {
        const long v = javaFrame->getOperandStack()->popLong();
        slot->setLongValue(v);
    } else if (descriptor == "D") {
        const double v = javaFrame->getOperandStack()->popDouble();
        slot->setDoubleValue(v);
    } else if (Utils::StringUtils::startsWith(descriptor, "L") || Utils::StringUtils::startsWith(descriptor, "[")) {
        Runtime::Object* v = javaFrame->getOperandStack()->popObject();
        slot->setRefs(v);
    }
    fieldJavaClass->setStaticVars(slotsVector);
}

} // namespace Instruction
