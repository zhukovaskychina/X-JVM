//
// Created by zhukovasky on 2020/8/18.
//

#ifndef JVM_METHODAREA_H
#define JVM_METHODAREA_H
//方法区
#include "../classfile/ClassFile.h"
#include "../classfile/ConstantPools.h"
#include "ClassLoader.h"


namespace Runtime {

    class JavaClass;
    class ClassPath;
    class ClassLoader;
    class MethodArea {

    public:
        MethodArea();

        JavaClass* loadJavaClass(std::string javaClassName);

        void loadPrimitiveAllJavaClasses();

        void loadPrimitiveJavaClass(std::string javaClassName);

        void loadBasicJavaClass(std::string javaClassName);

        JavaClass* loadClass(std::string className);

        JavaClass* loadArrayClass(std::string className);

        JavaClass* loadNonArrayClass(std::string className);

        Heap::Entry* readClass(std::string className);

        JavaClass* defineClass(Lang::ByteQueue* data);

        void resolveSuperClass(Runtime::JavaClass *javaClass);

        void resolveInterfaces(JavaClass *javaClass);

        void link(JavaClass *javaClass);

        void prepare(JavaClass *javaClass);



    private:
        Runtime::ClassLoader *systemClassLoader;
        Runtime::ClassLoader *userClassLoader;
        ClassPath *classPath;
        std::map<string,JavaClass*> classTable;
        void initBootStrapClassLoader(std::string jreOptions);

        void calculateInstanceFieldSlotsId(JavaClass *javaClass);

        void calculateStaticFieldSlotsId(JavaClass *javaClass);

        void allocateAndInitStaticVars(JavaClass *javaClass);


    };
}

#endif //JVM_METHODAREA_H
