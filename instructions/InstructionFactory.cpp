//
// Created by zhukovasky on 2020/8/29.
//

#include "InstructionFactory.h"
#include "constants/Const.h"
#include "constants/NoOp.h"
#include "maths/Maths.h"
#include "store/Store.h"
#include "load/Load.h"
#include "load/Aload.h"
#include "control/Return.h"
#include "control/GoTo.h"
#include "control/AThrow.h"
#include "control/Wide.h"
#include "control/Monitor.h"
#include "constants/BiSiPush.h"
#include "comparisons/Comparisons.h"
#include "reference/GetStatic.h"
#include "reference/PutStatic.h"
#include "reference/InvokeStatic.h"
#include "reference/FieldAccess.h"
#include "reference/Array.h"
#include "constants/Ldc.h"
#include "reference/NewRef.h"
#include "reference/CheckCast.h"
#include "reference/InstanceOf.h"
#include "reference/MultiANewArray.h"
#include "stack/StackCommand.h"
#include "store/AStore.h"

#include <iostream>

Instruction::Instructions* Instruction::InstructionFactory::newInstruction(u1 opCode) {
    Instructions* instructions = nullptr;
    switch (opCode) {
        case 0x00:
            instructions = new Instruction::NoOp;
            break;
        case 0x01:
            instructions = new Instruction::ACONST_NULL;
            break;
        case 0x02:
            instructions = new Instruction::ICONST_M1;
            break;
        case 0x03:
            instructions = new Instruction::ICONST_0;
            break;
        case 0x04:
            instructions = new Instruction::ICONST_1;
            break;
        case 0x05:
            instructions = new Instruction::ICONST_2;
            break;
        case 0x06:
            instructions = new Instruction::ICONST_3;
            break;
        case 0x07:
            instructions = new Instruction::ICONST_4;
            break;
        case 0x08:
            instructions = new Instruction::ICONST_5;
            break;
        case 0x09:
            instructions = new Instruction::LCONST_0;
            break;
        case 0x0a:
            instructions = new Instruction::LCONST_1;
            break;
        case 0x0b:
            instructions = new Instruction::FCONST_0;
            break;
        case 0x0c:
            instructions = new Instruction::FCONST_1;
            break;
        case 0x0d:
            instructions = new Instruction::FCONST_2;
            break;
        case 0x10:
            instructions = new Instruction::BiPush;
            break;
        case 0x11:
            instructions = new Instruction::SiPush;
            break;
        case 0x12:
            instructions = new Instruction::LDC;
            break;
        case 0x15:
            instructions = new Instruction::ILoad;
            break;
        case 0x16:
            instructions = new Instruction::LLoad;
            break;
        case 0x17:
            instructions = new Instruction::FLoad;
            break;
        case 0x18:
            instructions = new Instruction::DLoad;
            break;
        case 0x19:
            instructions = new Instruction::ALoad;
            break;
        case 0x1a:
            instructions = new Instruction::ILoad0;
            break;
        case 0x1b:
            instructions = new Instruction::ILoad1;
            break;
        case 0x1c:
            instructions = new Instruction::ILoad2;
            break;
        case 0x1d:
            instructions = new Instruction::ILoad3;
            break;
        case 0x1e:
            instructions = new Instruction::ILoad4;
            break;
        case 0x1f:
            instructions = new Instruction::ILoad5;
            break;
        case 0x20:
            instructions = new Instruction::ILoad6;
            break;
        case 0x21:
            instructions = new Instruction::ILoad7;
            break;
        case 0x2a:
            instructions = new Instruction::ALoad0;
            break;
        case 0x2b:
            instructions = new Instruction::ALoad1;
            break;
        case 0x2c:
            instructions = new Instruction::ALoad2;
            break;
        case 0x2d:
            instructions = new Instruction::ALoad3;
            break;
        case 0x36:
            instructions = new Instruction::IStore;
            break;
        case 0x37:
            instructions = new Instruction::LStore;
            break;
        case 0x38:
            instructions = new Instruction::FStore;
            break;
        case 0x39:
            instructions = new Instruction::DStore;
            break;
        case 0x3a:
            instructions = new Instruction::AStore;
            break;
        case 0x3b:
            instructions = new Instruction::IStore0;
            break;
        case 0x3c:
            instructions = new Instruction::IStore1;
            break;
        case 0x3d:
            instructions = new Instruction::IStore2;
            break;
        case 0x3e:
            instructions = new Instruction::IStore3;
            break;
        case 0x4b:
            instructions = new Instruction::AStore0;
            break;
        case 0x4c:
            instructions = new Instruction::AStore1;
            break;
        case 0x4d:
            instructions = new Instruction::AStore2;
            break;
        case 0x4e:
            instructions = new Instruction::AStore3;
            break;
        case 0x4f:
            instructions = new Instruction::IAStore;
            break;
        case 0x57:
            instructions = new Instruction::Pop;
            break;
        case 0x59:
            instructions = new Instruction::Dup;
            break;
        case 0x60:
            instructions = new Instruction::IAdd;
            break;
        case 0x64:
            instructions = new Instruction::ISub;
            break;
        case 0x68:
            instructions = new Instruction::IMul;
            break;
        case 0x6c:
            instructions = new Instruction::IDiv;
            break;
        case 0x70:
            instructions = new Instruction::IRem;
            break;
        case 0x74:
            instructions = new Instruction::INeg;
            break;
        case 0x78:
            instructions = new Instruction::ISHL;
            break;
        case 0x7a:
            instructions = new Instruction::ISHR;
            break;
        case 0x7c:
            instructions = new Instruction::IUSHR;
            break;
        case 0x7e:
            instructions = new Instruction::IAnd;
            break;
        case 0x80:
            instructions = new Instruction::Ior;
            break;
        case 0x82:
            instructions = new Instruction::IXor;
            break;
        case 0x84:
            instructions = new Instruction::IInc;
            break;
        case 0x99:
            instructions = new Instruction::IfEq;
            break;
        case 0x9a:
            instructions = new Instruction::IfNe;
            break;
        case 0x9b:
            instructions = new Instruction::IfLt;
            break;
        case 0x9c:
            instructions = new Instruction::IfGe;
            break;
        case 0x9d:
            instructions = new Instruction::IfGt;
            break;
        case 0x9e:
            instructions = new Instruction::IfLe;
            break;
        case 0x9f:
            instructions = new Instruction::IfIcmpeq;
            break;
        case 0xa0:
            instructions = new Instruction::IfIcmpne;
            break;
        case 0xa1:
            instructions = new Instruction::IfIcmplt;
            break;
        case 0xa2:
            instructions = new Instruction::IfIcmpge;
            break;
        case 0xa3:
            instructions = new Instruction::IfIcmpgt;
            break;
        case 0xa4:
            instructions = new Instruction::IfIcmple;
            break;
        case 0xa5:
            instructions = new Instruction::IfAcmpeq;
            break;
        case 0xa6:
            instructions = new Instruction::IfAcmpne;
            break;
        case 0xa7:
            instructions = new Instruction::GoTo;
            break;
        case 0xac:
            instructions = new Instruction::IReturn;
            break;
        case 0xad:
            instructions = new Instruction::LReturn;
            break;
        case 0xae:
            instructions = new Instruction::FReturn;
            break;
        case 0xaf:
            instructions = new Instruction::DReturn;
            break;
        case 0xb0:
            instructions = new Instruction::AReturn;
            break;
        case 0xb1:
            instructions = new Instruction::ReturnInst();
            break;
        case 0xb2:
            instructions = new Instruction::GetStatic;
            break;
        case 0xb3:
            instructions = new Instruction::PutStatic;
            break;
        case 0xb4:
            instructions = new Instruction::GetField;
            break;
        case 0xb5:
            instructions = new Instruction::PutField;
            break;
        case 0xb6:
            instructions = new Instruction::InvokeVirtual;
            break;
        case 0xb7:
            instructions = new Instruction::InvokeSpecial;
            break;
        case 0xb8:
            instructions = new Instruction::InvokeStatic;
            break;
        case 0xb9:
            instructions = new Instruction::InvokeInterface;
            break;
        case 0xbb:
            instructions = new Instruction::NewRef;
            break;
        case 0xbc:
            instructions = new Instruction::NewArray;
            break;
        case 0xbd:
            instructions = new Instruction::ANewArray;
            break;
        case 0xbe:
            instructions = new Instruction::ArrayLength;
            break;
        case 0xbf:
            instructions = new Instruction::AThrow;
            break;
        case 0xc0:
            instructions = new Instruction::CheckCast;
            break;
        case 0xc1:
            instructions = new Instruction::InstanceOf;
            break;
        case 0xc2:
            instructions = new Instruction::MonitorEnter;
            break;
        case 0xc3:
            instructions = new Instruction::MonitorExit;
            break;
        case 0xc4:
            instructions = new Instruction::WideInst;
            break;
        case 0xc5:
            instructions = new Instruction::MultiANewArray;
            break;
        case 0xc6:
            instructions = new Instruction::IfNull;
            break;
        case 0xc7:
            instructions = new Instruction::IfNonNull;
            break;
        case 0x32:
            instructions = new Instruction::AAload;
            break;
        case 0x53:
            instructions = new Instruction::AAStore;
            break;
        default:
            std::cerr << "[X-JVM] Unsupported opcode: 0x" << std::hex << static_cast<int>(opCode)
                      << std::dec << std::endl;
            instructions = nullptr;
            break;
    }

    return instructions;
}
