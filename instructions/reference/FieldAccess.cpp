//
// getfield / putfield
//

#include "FieldAccess.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../runtime/heap/ClassMember.h"
#include "../../runtime/Object.h"
#include "../../runtime/Slots.h"
#include "../../utils/StringUtils.h"

#include <stdexcept>

namespace Instruction {

static Runtime::Slots* instanceSlot(Runtime::Object* obj, int slotId) {
    if (!obj) {
        return nullptr;
    }
    Runtime::Slots* arr = obj->getFields();
    if (!arr || slotId < 0) {
        return nullptr;
    }
    return &arr[slotId];
}

void GetField::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::JavaClass* cur = javaFrame->getMethod()->getJavaClass();
    Runtime::Heap::FieldRef* fieldRef =
        cur->getRuntimeConstantsPool()->getIndex(this->getIndex())->getFieldRef();
    Runtime::Heap::FieldInfo* fi = fieldRef->resolvedFields();
    if (!fi) {
        throw std::runtime_error("GetField: unresolved field");
    }
    if (fi->isStatic()) {
        throw std::runtime_error("GetField: IncompatibleClassChangeError (static field)");
    }
    Runtime::Object* obj = javaFrame->getOperandStack()->popObject();
    if (!obj) {
        throw std::runtime_error("GetField: NullPointerException");
    }
    const int sid = fi->getSlotId();
    Runtime::Slots* slot = instanceSlot(obj, sid);
    if (!slot) {
        throw std::runtime_error("GetField: bad instance slot");
    }
    const std::string& d = fi->getDescriptor();
    if (d == "Z" || d == "B" || d == "C" || d == "S" || d == "I") {
        javaFrame->getOperandStack()->pushInt(slot->getNums());
    } else if (d == "F") {
        javaFrame->getOperandStack()->pushFloat(slot->getFloatValue());
    } else if (d == "J") {
        javaFrame->getOperandStack()->pushLong(slot->getLongValue());
    } else if (d == "D") {
        javaFrame->getOperandStack()->pushDouble(slot->getDoubleValue());
    } else if (Utils::StringUtils::startsWith(d, "L") || Utils::StringUtils::startsWith(d, "[")) {
        javaFrame->getOperandStack()->pushRef(slot->getRefs());
    } else {
        throw std::runtime_error("GetField: unsupported descriptor");
    }
}

void PutField::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::JavaClass* cur = javaFrame->getMethod()->getJavaClass();
    Runtime::Heap::FieldRef* fieldRef =
        cur->getRuntimeConstantsPool()->getIndex(this->getIndex())->getFieldRef();
    Runtime::Heap::FieldInfo* fi = fieldRef->resolvedFields();
    if (!fi) {
        throw std::runtime_error("PutField: unresolved field");
    }
    if (fi->isStatic()) {
        throw std::runtime_error("PutField: IncompatibleClassChangeError (static field)");
    }
    const std::string& d = fi->getDescriptor();
    Runtime::Object* obj = nullptr;
    if (d == "Z" || d == "B" || d == "C" || d == "S" || d == "I") {
        int v = javaFrame->getOperandStack()->popInt();
        obj = javaFrame->getOperandStack()->popObject();
        Runtime::Slots* slot = instanceSlot(obj, fi->getSlotId());
        if (!obj || !slot) {
            throw std::runtime_error("PutField: NullPointerException");
        }
        slot->setNums(v);
    } else if (d == "F") {
        float v = javaFrame->getOperandStack()->popFloat();
        obj = javaFrame->getOperandStack()->popObject();
        Runtime::Slots* slot = instanceSlot(obj, fi->getSlotId());
        if (!obj || !slot) {
            throw std::runtime_error("PutField: NullPointerException");
        }
        slot->setFloatValue(v);
    } else if (d == "J") {
        long v = javaFrame->getOperandStack()->popLong();
        obj = javaFrame->getOperandStack()->popObject();
        Runtime::Slots* slot = instanceSlot(obj, fi->getSlotId());
        if (!obj || !slot) {
            throw std::runtime_error("PutField: NullPointerException");
        }
        slot->setLongValue(v);
    } else if (d == "D") {
        double v = javaFrame->getOperandStack()->popDouble();
        obj = javaFrame->getOperandStack()->popObject();
        Runtime::Slots* slot = instanceSlot(obj, fi->getSlotId());
        if (!obj || !slot) {
            throw std::runtime_error("PutField: NullPointerException");
        }
        slot->setDoubleValue(v);
    } else if (Utils::StringUtils::startsWith(d, "L") || Utils::StringUtils::startsWith(d, "[")) {
        Runtime::Object* val = javaFrame->getOperandStack()->popObject();
        obj = javaFrame->getOperandStack()->popObject();
        Runtime::Slots* slot = instanceSlot(obj, fi->getSlotId());
        if (!obj || !slot) {
            throw std::runtime_error("PutField: NullPointerException");
        }
        slot->setRefs(val);
    } else {
        throw std::runtime_error("PutField: unsupported descriptor");
    }
}

} // namespace Instruction
