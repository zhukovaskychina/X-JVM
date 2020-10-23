//
// Created by zhukovasky on 2020/8/19.
//

#include "JavaHeap.h"
namespace Runtime{

    Object *JavaHeap::createJavaObject(JavaClass *javaClass) {
        Object *object=new Object();
        object->setJavaClass(javaClass);
        object->setData(nullptr);
        this->youngList.insert(object);
        return object;
    }

    Object *JavaHeap::createJavaArrayObject(JavaClass *javaClass) {
        Object *object=new Object();
        object->setJavaClass(javaClass);
        object->setData(nullptr);
        this->eldenList.insert(object);
        return nullptr;
    }
}