//
// Created by zhukovasky on 2020/8/11.
//

#ifndef JAVAP_OBJECT_H
#define JAVAP_OBJECT_H


#include <list>
#include <iostream>
namespace Runtime{
    class JavaClass;
    class Slots;
    class Object {
    public:
        Object(JavaClass *javaClass);

        Slots *getFields() const;

        void setFields(Slots *fields);

        JavaClass *getJavaClass() const;

        void setJavaClass(JavaClass *javaClass);

        virtual ~Object();


        int getArrayLength();

        /** 原始数组长度（`createArray`）；非数组为 -1。 */
        void setArrayLength(int n);

        const std::string &getObjectType() const;

        void setObjectType(const std::string &objectType);

        Object();

        void *getData() const;

        void setData(void *data);

        /** GC tri-color / mark-sweep: mark bit for reachability. */
        bool isGcMarked() const;
        void setGcMarked(bool marked);
        void clearGcMark();

        /** 数组与元素区由 JavaClass::createArray 单次 malloc，由 Object::deleteInstance 释放。 */
        bool isArrayCoallocated() const;
        void setArrayCoallocated(bool v);

        /** 替代 delete：支持数组共分配块。普通对象等价于 delete。 */
        static void deleteInstance(Object* o);

    private:
        Slots* fields;
        JavaClass *javaClass;
        std::string objectType;
        bool couldBeRemoved;
        void* data;
        int age;
        long address;
        bool gcMarked_{false};
        int arrayLength_{-1};
        bool arrayCoallocated_{false};
    };
}



#endif //JAVAP_OBJECT_H
