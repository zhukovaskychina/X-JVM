//
// Created by zhukovasky on 2020/8/19.
//

#ifndef JVM_CLASSMEMBER_H
#define JVM_CLASSMEMBER_H

#include <list>
#include "../../common/Internal.h"

#include "../../classfile/ConstantPools.h"
#include "../../classfile/ClassFile.h"
#include "../ClassLoader.h"
#include "../Slots.h"
#include "ConstantsPoolObject.h"

using namespace std;
namespace Runtime{
    class JavaClass;
    class ConstantsPoolObject;
    namespace Heap {
        class ClassMember {
        public:
            ClassMember();

            virtual ~ClassMember()=0;

            virtual u2 getAccessFlags() const=0;

            virtual void setAccessFlags(u2 accessFlags)=0;

            virtual const std::string &getName() const=0;

            virtual void setName(const std::string &name)=0;

            virtual const std::string &getDescriptor() const=0;

            virtual void setDescriptor(const string &descriptor)=0;

            virtual Runtime::JavaClass *getJavaClass() const=0;

            virtual void setJavaClass(JavaClass *javaClass)=0;

            virtual bool isPublic()=0;

            virtual bool isFinal()=0;

            virtual bool isStatic()=0;

            virtual bool isInterface()=0;

            virtual bool isAbstract()=0;

            virtual bool isSynthetic()=0;

            virtual bool isAnnotation()=0;

            virtual bool isEnum()=0;

        private:
            u2 accessFlags;
            std::string name;
            std::string descriptor;
            JavaClass* javaClass;
        };

        class FieldInfo:public ClassMember{
        public:
            int getSlotId() const;

            void setSlotId(int slotId);

            bool isLongOrDouble();

            int getConstantValueIndex() const;

            void setConstantValueIndex(int constantValueIndex);

            u2 getAccessFlags() const override;

            void setAccessFlags(u2 accessFlags) override;

            const string &getName() const override;

            void setName(const std::string &name) override;

            const string &getDescriptor() const override;

            void setDescriptor(const string &descriptor) override;

            JavaClass *getJavaClass() const override;

            void setJavaClass(JavaClass *javaClass) override;

            bool isPublic() override;

            bool isFinal() override;

            bool isStatic() override;

            bool isInterface() override;

            bool isAbstract() override;

            bool isSynthetic() override;

            bool isAnnotation() override;

            bool isEnum() override;

        private:
            int constantValueIndex;
            int slotId;
        };
        //方法信息
        class Method:public ClassMember{
        public:
            Method();

            virtual ~Method();

            u1 getMaxStack() const;

            void setMaxStack(u1 maxStack);

            u1 getMaxLocal() const;

            void setMaxLocal(u1 maxLocal);

            u1 *getCode() const;

            void setCode(u1 *code);

            bool isSynchronized();

            bool isStrict();

            bool isNative();

            bool isAbstract();

            bool isBridge();

            bool isVarargs();

            int argsSlotCount();

            void setArgsSlotCount(int argsSlotCount);

            int calcArgsSlotCount(std::string descriptor);

            u2 getAccessFlags() const override;

            void setAccessFlags(u2 accessFlags) override;

            const string &getName() const override;

            void setName(const std::string &name) override;

            const string &getDescriptor() const override;

            void setDescriptor(const string &descriptor) override;

            JavaClass *getJavaClass() const override;

            void setJavaClass(JavaClass *javaClass) override;

            bool isPublic() override;

            bool isFinal() override;

            bool isStatic() override;

            bool isInterface() override;

            bool isSynthetic() override;

            bool isAnnotation() override;

            bool isEnum() override;

        private:
            u1 maxStack;
            u1 maxLocal;
            u1* code;
            int argsCount; //参数
        };

        class RuntimeConstantsPool{
        public:
            RuntimeConstantsPool();

            virtual ~RuntimeConstantsPool();

            JavaClass *getJavaClass() const;

            void setJavaClass(JavaClass *javaClass);


            const vector<ConstantsPoolObject *> &getConstantsPoolObjectVector() const;

            void setConstantsPoolObjectVector(const vector<ConstantsPoolObject *> &constantsPoolObjectVector);

            ConstantsPoolObject* getIndex(int index);
        private:

            JavaClass* javaClass;

            std::vector<ConstantsPoolObject*> constantsPoolObjectVector;


        };

        //类符号 引用
        class SymRef{
        public:
            SymRef();

            virtual ~SymRef()=0;

            RuntimeConstantsPool *getConstantsPool() const;

            void setConstantsPool(RuntimeConstantsPool *constantsPool);

            virtual const string &getClassName() const=0;

            virtual void setClassName(const string &className)=0;

            virtual JavaClass *getJavaClass() const=0;

            virtual void setJavaClass(JavaClass *javaClass)=0;

            virtual JavaClass* resolvedJavaClass()=0;

            virtual JavaClass* resolveClassRef()=0;


            virtual bool isAccessiableTo(JavaClass* targetClass)=0;
        private:
            RuntimeConstantsPool* constantsPool;
            std::string className;
            JavaClass* javaClass;
        };
        //类符号
        class ClassRef:public SymRef{
        public:
            ClassRef();

            virtual ~ClassRef();

            const string &getClassName() const override;

            void setClassName(const string &className) override;

            JavaClass *getJavaClass() const override;

            void setJavaClass(JavaClass *javaClass) override;

            JavaClass *resolvedJavaClass() override;

            JavaClass *resolveClassRef() override;

            bool isAccessiableTo(JavaClass *targetClass) override;

        private:

        };

        //字段符号
        class MemberRef:public SymRef{
        public:
            MemberRef();

            virtual ~MemberRef();

            const string &getName() const;

            void setName(const string &name);

            const string getDescriptor() const;

            void setDescriptor(const string &descriptor);

            const string &getClassName() const override;

            void setClassName(const string &className) override;

            JavaClass *getJavaClass() const override;

            void setJavaClass(JavaClass *javaClass) override;

            JavaClass *resolvedJavaClass() override;

            JavaClass *resolveClassRef() override;

            bool isAccessiableTo(JavaClass *targetClass) override;

        private:

            std::string name;
            std::string descriptor;
        };

        class FieldRef:public MemberRef{
        public:
            FieldRef();

            virtual ~FieldRef();

            FieldInfo *getFieldInfo() const;

            void setFieldInfo(FieldInfo *fieldInfo);


            FieldInfo* resolvedFields();

            void resolveFieldRef();


            FieldInfo* lookupField(JavaClass* javaClass,std::string name ,std::string descriptor);

            bool isAccessibleTo(JavaClass* targetClass);
        private:
            FieldInfo* fieldInfo;

            bool isAccessibleTo();
        };

        class MethodRef:public MemberRef{
        public:
            MethodRef();

            virtual ~MethodRef();

            Method *getMethod() const;

            void setMethod(Method *method);

            void parseMethods(ClassFile::ClassFile *classFile);


            void resolveMethodRef();

            Method* resolvedMethod();

            Method* lookupMethod(JavaClass* javaClass,std::string methodName,std::string descriptor);

            //类内部查找
            Method* lookupMethodInClass(JavaClass* javaClass,std::string methodName,std::string descriptor);

            //接口内部查找
            Method* lookupMethodInInterface(std::list<JavaClass*> interfaceJavaClasses,std::string methodName,std::string descriptor);


        private:

            Method* method;

            std::list<Method*> methodList;
        };


        //接口方法符号引用
        class InterfaceMethodRef :public MemberRef{
        public:
            InterfaceMethodRef();

            virtual ~InterfaceMethodRef();


            Method *getMethod() const;

            void setMethod(Method *method);

            Method* resolvedInterfaceMethod();

            void resolveInterfaceMethodRef();

            Method* lookupInterfaceMethod(JavaClass* javaClass,std::string methodName,std::string descriptor);
        private:
            Method* method;
        };
    }
  }




#endif //JVM_CLASSMEMBER_H
