#include "Wide.h"
#include "../load/Load.h"
#include "../store/Store.h"

#include <stdexcept>

namespace Instruction {

void WideInst::fetchOperands(Instruction::ByteCodeReader* byteCodeReader) {
    mod_ = byteCodeReader->readU1();
    if (mod_ == 0x84) {
        idx_ = byteCodeReader->readU2();
        c_ = byteCodeReader->readU2();
    } else {
        idx_ = byteCodeReader->readU2();
    }
}

void WideInst::execute(Runtime::JavaFrame* javaFrame) {
    const long li = static_cast<long>(idx_);
    switch (mod_) {
    case 0x15: { // iload
        int v = javaFrame->getLocalVariableTables()->getInt(li);
        javaFrame->getOperandStack()->pushInt(v);
        break;
    }
    case 0x16: { // lload
        long v = javaFrame->getLocalVariableTables()->getLong(li);
        javaFrame->getOperandStack()->pushLong(v);
        break;
    }
    case 0x17: { // fload
        float v = javaFrame->getLocalVariableTables()->getFloat(li);
        javaFrame->getOperandStack()->pushFloat(v);
        break;
    }
    case 0x18: { // dload
        double v = javaFrame->getLocalVariableTables()->getDouble(li);
        javaFrame->getOperandStack()->pushDouble(v);
        break;
    }
    case 0x19: { // aload
        Runtime::Object* v = javaFrame->getLocalVariableTables()->getRef(li);
        javaFrame->getOperandStack()->pushRef(v);
        break;
    }
    case 0x36: { // istore
        int v = javaFrame->getOperandStack()->popInt();
        javaFrame->getLocalVariableTables()->setInt(li, v);
        break;
    }
    case 0x37: { // lstore
        long v = javaFrame->getOperandStack()->popLong();
        javaFrame->getLocalVariableTables()->setLong(li, v);
        break;
    }
    case 0x38: { // fstore
        float v = javaFrame->getOperandStack()->popFloat();
        javaFrame->getLocalVariableTables()->setFloat(li, v);
        break;
    }
    case 0x39: { // dstore
        double v = javaFrame->getOperandStack()->popDouble();
        javaFrame->getLocalVariableTables()->setDouble(li, v);
        break;
    }
    case 0x3a: { // astore
        Runtime::Object* v = javaFrame->getOperandStack()->popObject();
        javaFrame->getLocalVariableTables()->setRefs(li, v);
        break;
    }
    case 0x84: { // iinc
        int cur = javaFrame->getLocalVariableTables()->getInt(li);
        cur += static_cast<int16_t>(c_);
        javaFrame->getLocalVariableTables()->setInt(li, cur);
        break;
    }
    default:
        throw std::runtime_error("wide: unsupported modified opcode");
    }
}

} // namespace Instruction
