//
// Created by zhukovasky on 2020/8/19.
//

#ifndef JVM_JAVACLASS_H
#define JVM_JAVACLASS_H


#include <string>
#include "../../common/Internal.h"
#include "../../classfile/ConstantPools.h"
#include "ClassMember.h"
#include "../ClassLoader.h"
#include "../Slots.h"
#include "../../classfile/attributeInfo/Code_attribute.h"
#include "../../classfile/attributeInfo/ConstantValue.h"
#include "../JavaThread.h"

namespace Runtime{
    class Heap::RuntimeConstantsPool;
    class ClassLoader;
    class Heap::FieldInfo;
    class Method;
    class Slots;
    class FieldRef;
    class classRef;
    class MethodRef;
    class SymRef;

    class JavaClass {
    public:
        JavaClass(ClassFile::ClassFile* classFile);

        bool isPublic();

        bool isFinal();

        bool isSuper();

        bool isInterface();

        bool isAbstract();

        bool isSynthetic();

        bool isAnnotation();

        bool isEnum();

        bool isArray();

        Object* createArray(int length);

        bool isAccessibleTo(JavaClass* target);

        std::string getPackageName();

        Runtime::Heap::Method* getMainMethod();

        std::list<Runtime::Heap::Method*> getStaticMethod();

        Heap::Method* getStaticMethod(std::string name,std::string descriptor);

        Heap::Method *getMethodInfos() const;

        void setMethodInfos(Heap::Method *methodInfos);

        ClassLoader *getClassLoader() const;

        void setClassLoader(ClassLoader *classLoader);

        std::string getThisClassName();

        JavaClass *getSuperClass() const;

        void setSuperClass(JavaClass *superClass);

        const string &getSuperClassName() const;

        void parseThisClassName();

        u1 getInstanceCount() const;

        void setInstanceCount(u1 instanceCount);

        u1 getStaticCount() const;

        void setStaticCount(u1 staticCount);

        std::vector<Slots*> getStaticVars() const;

        void setStaticVars(std::vector<Slots*> staticVars);



        Heap::RuntimeConstantsPool *getRuntimeConstantsPool() const;

        Object* createNewJavaObjectInstance();

        const vector<std::string> &getInterfacesNames() const;

        const list<Heap::Method *> &getMethodInfosList() const;

        const list<Heap::FieldInfo *> &getFieldInfoList() const;



        const list<JavaClass *> getInterfaces() const;

        void setInterfaces(const list<JavaClass *> &interfaces);

        JavaClass(std::string className,ClassLoader *classLoader,bool initStarted);

        bool initStarted();

        void startInit();

        void initJavaClass(JavaThread* javaThread,JavaClass* javaClass);

        void scheduleClinit(JavaThread* javaThread,JavaClass* javaClass);

        void initSuperClass(JavaThread* javaThread,JavaClass* javaClass);

        Heap::Method* getClinitMethod();
    private:

        void parseInterfaces();

        void initRuntimeConstantsPool(ClassFile::ConstantPoolsList* constantPoolsList);

        std::string getUTF8(u2 index,ClassFile::ConstantPoolsList *classFileConstantsPools);

        std::string getClassName(u2 index,ClassFile::ConstantPoolsList *constantPools);

        std::string getDescriptor(u2 index,ClassFile::ConstantPoolsList *constantPools);

        std::string getFieldOrMethodName(u2 index,ClassFile::ConstantPoolsList *pConstantPools);

        void parseMethods(std::list<ClassFile::MethodInfo*> methodInfoList);

        void parseFields();



        u2 accessFlags;
        std::string javaClassName;
        std::string superClassName;
        std::vector<std::string> interfacesNames;
        Heap::RuntimeConstantsPool *runtimeConstantsPool;
        std::list<Heap::Method*> methodInfosList;
        std::list<Heap::Method*> staticMethodsList;
        std::list<Heap::FieldInfo*> fieldInfoList;
        std::vector<Slots*> staticVars;
        JavaClass* superClass;
        u2 superClassCount;

        //实力话个数
        u1 instanceCount;
        u1 staticCount;
        ClassLoader *classLoader;


        ClassFile::ClassFile* classFile;

        ClassFile::ConstantPoolsList* constantPoolsList;

        std::string thisClassName;

        std::list<JavaClass*> interfaces;

        ClassFile::Code_attribute* getCodeAttrs(const ClassFile::MethodInfo *clsMethod) const;

        ClassFile::ConstantValue* getConstantValueAttribute(const ClassFile::FieldInfo *fieldInfo) const;
        bool initClassStarted;
    };
}



#endif //JVM_JAVACLASS_H
