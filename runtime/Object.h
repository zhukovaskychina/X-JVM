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

        const std::string &getObjectType() const;

        void setObjectType(const std::string &objectType);

        Object();

        void *getData() const;

        void setData(void *data);

    private:
        Slots* fields;
        JavaClass *javaClass;
        std::string objectType;
        bool couldBeRemoved;
        void* data;
        int age;
        long address;
    };
}



#endif //JAVAP_OBJECT_H
