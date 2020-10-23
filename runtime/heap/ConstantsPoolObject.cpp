//
// Created by zhukovasky on 2020/8/20.
//

#include "ConstantsPoolObject.h"
#include "ClassMember.h"
namespace Runtime{
    namespace Heap{
        template<typename T>
        T ConstantsPoolObject::getConstantPoolObjectValue() {
            if(this->constantType=="int"){
                return this->intValue;
            }
            if(this->constantType=="long"){
                return this->longValue;
            }
            if(this->constantType=="string"){
                return this->stringValue;
            }
            if(this->constantType=="float"){
                return this->floatValue;
            }
            if(this->constantType=="double"){
                return this->doubleValue;
            }
            if(this->constantType=="class"){
                return this->classRef;
            }
            if(this->constantType=="field"){
                return this->field;
            }
            if(this->constantType=="method"){
                return this->methodRef;
            }
            return nullptr;
        }

        template<typename T>
        void ConstantsPoolObject::setConstantPoolObjectValue(T value) {
            if(this->constantType=="int"){
                this->intValue=value;
                return;
            }
            if(this->constantType=="long"){
                this->longValue=value;
                return;
            }
            if(this->constantType=="string"){
                this->stringValue=value;
                return;
            }
            if(this->constantType=="float"){
                this->floatValue=value;
                return;
            }
            if(this->constantType=="double"){
                this->doubleValue=value;
                return;
            }
            if(this->constantType=="class"){
                this->classRef=value;
                return;
            }
            if(this->constantType=="field"){
                this->field=value;
                return;
            }
            if(this->constantType=="method"){
                this->methodRef=value;
                return;
            }
        }

        const string &ConstantsPoolObject::getConstantType() const {
            return constantType;
        }

        void ConstantsPoolObject::setConstantType(const string &constantType) {
            ConstantsPoolObject::constantType = constantType;
        }

        int ConstantsPoolObject::getIntValue() const {
            return intValue;
        }

        void ConstantsPoolObject::setIntValue(int intValue) {
            ConstantsPoolObject::intValue = intValue;
        }

        const string &ConstantsPoolObject::getStringValue() const {
            return stringValue;
        }

        void ConstantsPoolObject::setStringValue(const string &stringValue) {
            ConstantsPoolObject::stringValue = stringValue;
        }

        float ConstantsPoolObject::getFloatValue() const {
            return floatValue;
        }

        void ConstantsPoolObject::setFloatValue(float floatValue) {
            ConstantsPoolObject::floatValue = floatValue;
        }

        long ConstantsPoolObject::getLongValue() const {
            return longValue;
        }

        void ConstantsPoolObject::setLongValue(long longValue) {
            ConstantsPoolObject::longValue = longValue;
        }

        double ConstantsPoolObject::getDoubleValue() const {
            return doubleValue;
        }

        void ConstantsPoolObject::setDoubleValue(double doubleValue) {
            ConstantsPoolObject::doubleValue = doubleValue;
        }

        Heap::FieldRef *ConstantsPoolObject::getFieldRef() const {
            return field;
        }

        void ConstantsPoolObject::setFieldRef(Heap::FieldRef *field) {
            ConstantsPoolObject::field = field;
        }

        Heap::ClassRef *ConstantsPoolObject::getClassRef() const {
            return classRef;
        }

        void ConstantsPoolObject::setClassRef(Heap::ClassRef *classRef) {
            ConstantsPoolObject::classRef = classRef;
        }

        Heap::MethodRef *ConstantsPoolObject::getMethodRef() const {
            return methodRef;
        }

        void ConstantsPoolObject::setMethodRef(Heap::MethodRef *methodRef) {
            ConstantsPoolObject::methodRef = methodRef;
        }

        Heap::InterfaceMethodRef *ConstantsPoolObject::getInterfaceMethodRef() const {
            return interfaceMethodRef;
        }

        void ConstantsPoolObject::setInterfaceMethodRef(Heap::InterfaceMethodRef *interfaceMethodRef) {
            ConstantsPoolObject::interfaceMethodRef = interfaceMethodRef;
        }

    }
}