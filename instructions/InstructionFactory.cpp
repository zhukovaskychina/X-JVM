//
// Created by zhukovasky on 2020/8/29.
//

#include "InstructionFactory.h"
#include "constants/Const.h"
#include "constants/NoOp.h"
#include "maths/Maths.h"
#include "store/Store.h"
#include "load/Load.h"
#include "control/Return.h"
#include "control/GoTo.h"
#include "constants/BiSiPush.h"
#include "comparisons/Comparisons.h"
#include "reference/GetStatic.h"
#include "reference/InvokeStatic.h"
#include "reference/Array.h"
#include "constants/Ldc.h"
#include "reference/NewRef.h"
#include "stack/StackCommand.h"
#include "store/AStore.h"

#define PRINT(code) cout<<code<<endl;
Instruction::Instructions *Instruction::InstructionFactory::newInstruction(u1 opCode) {
    //cout<<opCode<<endl;
    Instructions* instructions;
    switch (opCode){
        case 0x00:{
            instructions=new Instruction::NoOp;

            break;
        }
        case 0x01:{
            instructions= new Instruction::ACONST_NULL;
            break;
        }
        case 0x02:{
            instructions=new Instruction::ICONST_M1;
            PRINT("ICONST_M1");
            break;
        }
        case 0x03:{
            instructions=new Instruction::ICONST_0;
            PRINT("ICONST_0");
            break;
        }
        case 0x04:{
            PRINT("ICONST_1");
            instructions=new Instruction::ICONST_1;
            break;
        }
        case 0x05:{
            PRINT("ICONST_2");
            instructions=new Instruction::ICONST_2;
            break;
        }
        case 0x06:{
            instructions=new Instruction::ICONST_3;
            break;
        }
        case 0x07:{
            instructions=new Instruction::ICONST_4;
            break;
        }
        case 0x08:{
            instructions=new Instruction::ICONST_5;
            break;
        }
        case 0x09:{
            instructions=new Instruction::LCONST_0;
            break;
        }
        case 0x0a:{
            instructions=new Instruction::LCONST_1;
            break;
        }
        case 0x0b:{
            instructions=new Instruction::FCONST_0;
            break;
        }
        case 0x0c:{
            instructions=new Instruction::FCONST_1;
            break;
        }
        case 0x0d:{
            instructions=new Instruction::FCONST_2;
            break;
        }
        case 0x10:{
            PRINT("BIPUSH")
            instructions=new Instruction::BiPush;
            break;
        }
        case 0x11:{
            PRINT("SIPUSH")
            instructions=new Instruction::SiPush;
            break;
        }
        case 0x12:{
            PRINT("LDC")
            instructions=new Instruction::LDC;
            break;
        }
        case 0x15:{
            instructions=new Instruction::FCONST_2;
            break;
        }
        case 0x16:{
            instructions=new Instruction::FCONST_2;
            break;
        }
        case 0x17:{
            instructions=new Instruction::FCONST_2;
            break;
        }
        case 0x18:{
            instructions=new Instruction::FCONST_2;
            break;
        }
        case 0x19:{
            instructions=new Instruction::FCONST_2;
            break;
        }
        case 0x1a:{
            instructions=new Instruction::ILoad0;
            break;
        }
        case 0x1b:{
            PRINT("ILOAD1");
            instructions=new Instruction::ILoad1;
            break;
        }
        case 0x1c:{
            PRINT("ILOAD2");
            instructions=new Instruction::ILoad2;
            break;
        }
        case 0x1d:{
            PRINT("ILOAD3");
            instructions=new Instruction::ILoad3;
            break;
        }
        case 0x2a:{
            instructions=new Instruction::ALoad0;
            break;
        }
        case 0x2b:{
            PRINT("ALOAD1")
            instructions=new Instruction::ALoad1;
            break;
        }
        case 0x3b:{
            instructions=new Instruction::IStore0;
            break;
        }
        case 0x3c:{
            PRINT("ISTORE1");
            instructions=new Instruction::IStore1;
            break;
        }
        case 0x3d:{
            PRINT("ISTORE2");
            instructions=new Instruction::IStore2;
            break;
        }
        case 0x3e:{
            PRINT("ISTORE3");
            instructions=new Instruction::IStore3;
            break;
        }
        case 0x4b:{
            instructions=new Instruction::AStore0;
            break;
        }
        case 0x4c:{
            PRINT("ASTORE1");
            instructions=new Instruction::AStore1;
            break;
        }
        case 0x4d:{
            instructions=new Instruction::AStore2;
            break;
        }
        case 0x4e:{
            instructions=new Instruction::AStore3;
            break;
        }
        case 0x4f:{
            instructions=new Instruction::IAStore;
            break;
        }
        case 0x59:{
            instructions=new Instruction::Dup;
            break;
        }

        case 0x60:{
            PRINT("IADD");
            instructions=new Instruction::IAdd;
            break;
        }
        case 0x84:{
            PRINT("IINC")
            instructions=new Instruction::IInc;
            break;
        }
        case 0xa7:{
            PRINT("GOTO")
            instructions=new Instruction::GoTo;
            break;
        }
        case 0xb1:{
            PRINT("RETURN")
            instructions=new Instruction::ReturnInst();
            break;
        }
        case 0xb2:{
            PRINT("getstatic")
            instructions=new Instruction::GetStatic;
            break;
        }
        case 0xb6:{
            PRINT("invokevirtual")
            instructions=new Instruction::InvokeVirtual;
            break;
        }
        case 0xbb:{
            instructions=new Instruction::NewRef;
            break;
        }
        case 0x9f:{
            PRINT("IfIcmeq")
            instructions=new Instruction::IfIcmpeq;
            break;
        }
        case 0xbc:{
            PRINT("NEWARRAY")
            instructions=new Instruction::NewArray;
            break;
        }
        case 0xa2:{
            PRINT("IFIcmge")
            instructions=new Instruction::IfIcmpge;
        }

        default:{
            PRINT("还有指令")
            exit(1);
            break;
        }

    }

    return instructions;
}
