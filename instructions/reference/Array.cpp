//
// Created by zhukovasky on 2020/9/2.
//

#include "Array.h"
#include "../../runtime/ExceptionHelpers.h"
#include "../../utils/PlatformCompat.h"
#include "../../runtime/heap/JavaClass.h"
#include "../../runtime/heap/ConstantsPoolObject.h"
#include "../../runtime/Object.h"
#include "../../common/JvmEnums.h"
#include <stdexcept>
#include <string>
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

    void ANewArray::execute(Runtime::JavaFrame *javaFrame) {
        int count = javaFrame->getOperandStack()->popInt();
        if (count < 0) {
            throw std::runtime_error("Negative array size (anewarray)");
        }
        Runtime::JavaClass *cur = javaFrame->getMethod()->getJavaClass();
        Runtime::Heap::RuntimeConstantsPool *cp = cur->getRuntimeConstantsPool();
        if (!cp) {
            throw std::runtime_error("anewarray: no constant pool");
        }
        Runtime::Heap::ConstantsPoolObject *po = cp->getIndex(this->getIndex());
        if (!po) {
            throw std::runtime_error("anewarray: bad pool index");
        }
        Runtime::Heap::ClassRef *cref = po->getClassRef();
        if (!cref) {
            throw std::runtime_error("anewarray: expected CONSTANT_Class");
        }
        Runtime::JavaClass *component = cref->resolvedJavaClass();
        Runtime::ClassLoader *cl = cur->getClassLoader();
        if (!component || !cl) {
            throw std::runtime_error("anewarray: resolve component failed");
        }
        const std::string &cn = component->getThisClassName();
        std::string desc = (!cn.empty() && cn[0] == '[') ? (std::string("[") + cn) : (std::string("[L") + cn + ";");
        Runtime::JavaClass *arrCl = cl->loadClass(desc);
        if (!arrCl) {
            throw std::runtime_error("anewarray: load array class failed");
        }
        Runtime::Object *arr = arrCl->createArray(count);
        javaFrame->getOperandStack()->pushRef(arr);
    }

    void ArrayLength::execute(Runtime::JavaFrame *javaFrame) {
        Runtime::Object *arr = javaFrame->getOperandStack()->popObject();
        if (!arr) {
            Runtime::throwNullPointerException(javaFrame);
            return;
        }
        Runtime::JavaClass *jc = arr->getJavaClass();
        if (!jc || !jc->isArray()) {
            throw std::runtime_error("arraylength: not an array");
        }
        javaFrame->getOperandStack()->pushInt(arr->getArrayLength());
    }
}