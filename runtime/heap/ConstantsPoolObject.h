//
// Created by zhukovasky on 2020/8/20.
//

#ifndef JVM_CONSTANTSPOOLOBJECT_H
#define JVM_CONSTANTSPOOLOBJECT_H

//定义常量池对象类型
#include "iostream"
#include <string>
#include "ClassMember.h"
using namespace std;

namespace Runtime{

    namespace Heap{
        class FieldRef;
        class ClassRef;
        class MethodRef;
        class InterfaceMethodRef;
        class SymRef;
        class ConstantsPoolObject {
            friend class FieldRef;
        public:

            template <typename T>
            T getConstantPoolObjectValue();

            template <typename T>
            void setConstantPoolObjectValue(T value);

            const string &getConstantType() const;

            void setConstantType(const string &constantType);

            int getIntValue() const;

            void setIntValue(int intValue);

            const string &getStringValue() const;

            void setStringValue(const string &stringValue);

            float getFloatValue() const;

            void setFloatValue(float floatValue);

            long getLongValue() const;

            void setLongValue(long longValue);

            double getDoubleValue() const;

            void setDoubleValue(double doubleValue);

            FieldRef *getFieldRef() const;

            void setFieldRef(FieldRef *field);

            ClassRef *getClassRef() const;

            void setClassRef(ClassRef *classRef);

            MethodRef *getMethodRef() const;

            void setMethodRef(MethodRef *methodRef);

            InterfaceMethodRef *getInterfaceMethodRef() const;

            void setInterfaceMethodRef(InterfaceMethodRef *interfaceMethodRef);

        private:
            std::string constantType;

            int intValue;

            std::string stringValue;

            float floatValue;

            long longValue;

            double doubleValue;

            FieldRef *field;

            ClassRef *classRef;

            MethodRef *methodRef;

            InterfaceMethodRef *interfaceMethodRef;

        };


    }

}





#endif //JVM_CONSTANTSPOOLOBJECT_H
