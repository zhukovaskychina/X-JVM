//
// Created by zhukovasky on 2020/9/2.
//

#include "Array.h"
#include "../../utils/PlatformCompat.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../common/JvmEnums.h"
namespace Instruction{
    void NewArray::fetchOperands(Instruction::ByteCodeReader *byteCodeReader) {
        this->atype=byteCodeReader->readU1();
    }

    void NewArray::execute(Runtime::JavaFrame *javaFrame) {
       int count= javaFrame->getOperandStack()->popInt();
        if(count<0){
#ifdef _MSC_VER
            __throw_future_error(1);
#else
            throw std::runtime_error("Negative array size");
#endif
        }
        Runtime::JavaClass* javaClass=javaFrame->getMethod()->getJavaClass();
        Runtime::JavaClass* arrayClass=getPrimitiveArrayClass(javaClass->getClassLoader(),this->atype);
       // arrayClass->createArray(count);

        javaFrame->getOperandStack()->pushRef(arrayClass->createArray(count));
    }

    Runtime::JavaClass *NewArray::getPrimitiveArrayClass(Runtime::ClassLoader *classLoader, u1 atype) {
        switch (atype){
            case T_BOOLEAN:{
                return classLoader->loadClass("[Z");
            }
            case T_BYTE:{
                return classLoader->loadClass("[B");
            }
            case T_CHAR:{
                return classLoader->loadClass("[C");
            }
            case T_DOUBLE:{
                return classLoader->loadClass("[D");
            }
            case T_FLOAT:{
                return classLoader->loadClass("[F");
            }
            case T_INT:{
                return classLoader->loadClass("[I");
            }
            case T_LONG:{
                return classLoader->loadClass("[J");
            }
            case T_SHORT:{
                return classLoader->loadClass("[S");
            }
            default:{
                break;
            }
        }
        return nullptr;
    }
}