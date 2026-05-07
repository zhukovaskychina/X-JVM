//
// Created by zhukovasky on 2020/8/11.
//

#include "Object.h"
#include "detail/ArrayCoallocLayout.h"
#include "../runtime/heap/JavaClass.h"

#include <cstdlib>

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

    Object::Object(JavaClass *javaClass)
        : fields(nullptr), javaClass(javaClass), couldBeRemoved(false), data(nullptr), age(0), address(0),
          gcMarked_(false), arrayLength_(-1) {
       int instanceCount=javaClass->getInstanceCount();
       (void)instanceCount;
    }

    Object::~Object() {
        if (arrayLength_ >= 0 && arrayCoallocated_) {
            data = nullptr;
            arrayLength_ = -1;
            arrayCoallocated_ = false;
        } else if (arrayLength_ >= 0 && data && javaClass) {
            if (javaClass->isPrimitiveArray()) {
                delete[] reinterpret_cast<uint8_t*>(data);
            } else {
                delete[] reinterpret_cast<Object**>(data);
            }
            data = nullptr;
        } else if (arrayLength_ < 0 && fields && javaClass) {
            const u1 n = javaClass->getInstanceCount();
            if (n > 0) {
                delete[] fields;
            }
            fields = nullptr;
        }
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
        if (arrayLength_ >= 0) {
            return arrayLength_;
        }
        return 0;
    }

    void Object::setArrayLength(int n) {
        arrayLength_ = n;
    }

    Object::Object() : arrayLength_{-1} {

    }

    bool Object::isGcMarked() const {
        return gcMarked_;
    }

    void Object::setGcMarked(bool marked) {
        gcMarked_ = marked;
    }

    void Object::clearGcMark() {
        gcMarked_ = false;
    }

    bool Object::isArrayCoallocated() const {
        return arrayCoallocated_;
    }

    void Object::setArrayCoallocated(bool v) {
        arrayCoallocated_ = v;
    }

    void Object::deleteInstance(Object* o) {
        if (!o) {
            return;
        }
        if (o->isArrayCoallocated()) {
            unsigned char* raw =
                reinterpret_cast<unsigned char*>(o) - ArrayCoalloc::objectOffset();
            if (*reinterpret_cast<uint64_t*>(raw) != ArrayCoalloc::kMagic) {
                delete o;
                return;
            }
            o->~Object();
            std::free(raw);
            return;
        }
        delete o;
    }
}