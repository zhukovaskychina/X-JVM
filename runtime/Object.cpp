//
// Created by zhukovasky on 2020/8/11.
//

#include "Object.h"
#include "../runtime/heap/JavaClass.h"
#include "../common/Common.h"
namespace Runtime{
    Slots *Object::getFields() const {
        return fields;
    }

    void Object::setFields(Slots *fields) {
        Object::fields = fields;
    }

    JavaClass *Object::getJavaClass() const {
        return javaClass;
    }

    void Object::setJavaClass(JavaClass *javaClass) {
        Object::javaClass = javaClass;
    }

    Object::Object(JavaClass *javaClass) : javaClass(javaClass) {
       int instanceCount=javaClass->getInstanceCount();
       this->age=0;
    }

    Object::~Object() {

    }

    void *Object::getData() const {
        return data;
    }

    void Object::setData(void *data) {
        Object::data = data;
    }

    const string &Object::getObjectType() const {
        return objectType;
    }

    void Object::setObjectType(const string &objectType) {
        Object::objectType = objectType;
    }

    int Object::getArrayLength() {
        string type=this->fields->getSlotType();
        int length=GetArrayLength(this->fields);
        return length;
    }

    Object::Object() {

    }
}