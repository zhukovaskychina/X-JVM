//
// Created by zhukovasky on 2020/8/19.
//

#include "ClassMember.h"
#include "../../classfile/attributeInfo/Code_attribute.h"
#include "HeapAcessFlags.h"
#include "JavaClass.h"
#include "../../classfile/MethodInfo.h"
#include "MethodDescriptor.h"
#include "../HeapConstantUtils.h"
namespace Runtime{
    namespace Heap{
        ClassMember::ClassMember() {}

        ClassMember::~ClassMember() {

        }

        bool FieldRef::isAccessibleTo(JavaClass *targetClass) {
            return false;
        }

        u2 ClassMember::getAccessFlags() const {
            return accessFlags;
        }

        void ClassMember::setAccessFlags(u2 accessFlags) {
            ClassMember::accessFlags = accessFlags;
        }

        const string &ClassMember::getName() const {
            return name;
        }

        void ClassMember::setName(const string &name) {
            ClassMember::name = name;
        }

        const string &ClassMember::getDescriptor() const {
            return descriptor;
        }

        void ClassMember::setDescriptor(const string &descriptor) {
            ClassMember::descriptor = descriptor;
        }

        JavaClass *ClassMember::getJavaClass() const {
            return javaClass;
        }

        void ClassMember::setJavaClass(JavaClass *javaClass) {
            this->javaClass = javaClass;
        }

        bool ClassMember::isPublic() {
            return this->accessFlags&&ClassEnum ::ACC_PUBLIC;
        }

        bool ClassMember::isFinal() {
            return this->accessFlags&&ClassEnum ::ACC_FINAL;
        }

        bool ClassMember::isStatic() {
            return (this->accessFlags&ClassEnum ::ACC_STATIC)!=0;
        }

        bool ClassMember::isInterface() {
            return this->accessFlags&&ClassEnum ::ACC_INTERFACE;
        }

        bool ClassMember::isAbstract() {
            return this->accessFlags&&ClassEnum ::ACC_ABSTRACT;
        }

        bool ClassMember::isSynthetic() {
            return this->accessFlags&&ClassEnum ::ACC_SYNTHETIC;
        }

        bool ClassMember::isAnnotation() {
            return this->accessFlags&&ClassEnum ::ACC_ANNOTATION;
        }

        bool ClassMember::isEnum() {
            return this->accessFlags&&ClassEnum ::ACC_ENUM;
        }

        RuntimeConstantsPool::RuntimeConstantsPool() {}

        RuntimeConstantsPool::~RuntimeConstantsPool() {

        }

        JavaClass *RuntimeConstantsPool::getJavaClass() const {
            return javaClass;
        }

        void RuntimeConstantsPool::setJavaClass(JavaClass *javaClass) {
            RuntimeConstantsPool::javaClass = javaClass;
        }


        const vector<ConstantsPoolObject *> &RuntimeConstantsPool::getConstantsPoolObjectVector() const {
            return constantsPoolObjectVector;
        }

        void RuntimeConstantsPool::setConstantsPoolObjectVector(
                const vector<ConstantsPoolObject *> &constantsPoolObjectVector) {
            RuntimeConstantsPool::constantsPoolObjectVector = constantsPoolObjectVector;
        }

        ConstantsPoolObject *RuntimeConstantsPool::getIndex(int index) {
            return this->constantsPoolObjectVector[index-1];
        }

        Method::Method() {}

        Method::~Method() {

        }


        u1 Method::getMaxStack() const {
            return maxStack;
        }

        void Method::setMaxStack(u1 maxStack) {
            Method::maxStack = maxStack;
        }

        u1 Method::getMaxLocal() const {
            return maxLocal;
        }

        void Method::setMaxLocal(u1 maxLocal) {
            Method::maxLocal = maxLocal;
        }

        u1 *Method::getCode() const {
            return code;
        }

        void Method::setCode(u1 *code) {
            Method::code = code;
        }

        MemberRef::MemberRef() {}

        MemberRef::~MemberRef() {

        }



        const string &MemberRef::getName() const {
            return name;
        }

        void MemberRef::setName(const string &name) {
            MemberRef::name = name;
        }

        const string MemberRef::getDescriptor() const {
            return descriptor;
        }

        void MemberRef::setDescriptor(const string &descriptor) {
            MemberRef::descriptor = descriptor;
        }

        const string &MemberRef::getClassName() const {
            return SymRef::getClassName();
        }

        void MemberRef::setClassName(const string &className) {
            SymRef::setClassName(className);
        }

        JavaClass *MemberRef::getJavaClass() const {
            return SymRef::getJavaClass();
        }

        void MemberRef::setJavaClass(JavaClass *javaClass) {
            SymRef::setJavaClass(javaClass);
        }

        JavaClass *MemberRef::resolvedJavaClass() {
            return SymRef::resolvedJavaClass();
        }

        JavaClass *MemberRef::resolveClassRef() {
            return SymRef::resolveClassRef();
        }

        bool MemberRef::isAccessiableTo(JavaClass *targetClass) {
            return SymRef::isAccessiableTo(targetClass);
        }

        MethodRef::MethodRef() {}

        MethodRef::~MethodRef() {

        }

        void MethodRef::parseMethods(ClassFile::ClassFile *classFile) {

            std::list<ClassFile::MethodInfo*> methodInfoList=classFile->getMethodInfoList();

            std::list<ClassFile::MethodInfo*>::iterator methodInfoListIter;
            for (methodInfoListIter=methodInfoList.begin(); methodInfoListIter!=methodInfoList.end() ; ++methodInfoListIter) {
                ClassFile::MethodInfo *methodInfo=(*methodInfoListIter);
                ClassFile::ConstantPoolsList *constantPools=classFile->getConstantPools();
                Method *methodIter=new Method();
                methodIter->setName(HeapConstantUtils::getUTF8(methodInfo->getNameIndex(),constantPools));
                methodIter->setDescriptor(HeapConstantUtils::getUTF8(methodInfo->getDescriptorIndex(),constantPools));

                std::list<ClassFile::AttributesInfo*> attributesInfoList=methodInfo->getAttributesInfoList();

                std::list<ClassFile::AttributesInfo*>::iterator attributesInfoListIterator;
                for (attributesInfoListIterator=attributesInfoList.begin();attributesInfoListIterator!=attributesInfoList.end();attributesInfoListIterator++) {

                    ClassFile::AttributesInfo* iter=(*attributesInfoListIterator);
                    u2 attributeIndex=iter->getAttributeNameIndex();
                    std::string codeName=HeapConstantUtils::getUTF8(attributeIndex,constantPools);
                    if(codeName=="Code"){
                        ClassFile::Code_attribute *codeAttribute= reinterpret_cast<ClassFile::Code_attribute*>((*attributesInfoListIterator));
                        methodIter->setCode(codeAttribute->getCode());
                        methodIter->setMaxLocal(codeAttribute->getMaxLocals());
                        methodIter->setMaxStack(codeAttribute->getMaxStack());
                      //  methodIter->setArgsSlotCount();
                        break;
                    } else{
                        continue;
                    }
                }
                this->methodList.push_back(methodIter);
            }

        }

        Method *MethodRef::getMethod() const {
            return method;
        }

        void MethodRef::setMethod(Method *method) {
            MethodRef::method = method;
        }



        bool Method::isSynchronized() {
            return this->getAccessFlags()&&ClassEnum::ACC_SYNCHRONIZED;
        }

        bool Method::isStrict() {
            return this->getAccessFlags()&&ClassEnum ::ACC_STRICT;
        }

        bool Method::isNative() {
            return this->getAccessFlags()&&ClassEnum ::ACC_NATIVE;
        }

        bool Method::isAbstract() {
            return this->getAccessFlags()&&ClassEnum ::ACC_ABSTRACT;
        }

        bool Method::isBridge() {
            return this->getAccessFlags()&&ClassEnum ::ACC_BRIDGE;
        }

        bool Method::isVarargs() {
            return this->getAccessFlags()&&ClassEnum ::ACC_VARARGS;
        }

        int Method::argsSlotCount() {

            return this->argsCount;
        }

        void Method::setArgsSlotCount(int argsSlotCount) {
            this->argsCount=argsSlotCount;
        }

        int Method::calcArgsSlotCount(std::string descriptor) {
            MethodDescriptorParser* methodDescriptorParser;
            MethodDescriptor* methodDescriptor=methodDescriptorParser->parse(descriptor);
            std::list<string> parameterList=methodDescriptor->getParameterType();
            std::list<string>::iterator parameterListIterator;

            for (parameterListIterator=parameterList.begin(); parameterListIterator!=parameterList.end() ; ++parameterListIterator) {
                this->argsCount++;

                if((*parameterListIterator)=="J"||(*parameterListIterator)=="D"){
                    this->argsCount++;
                }
            }
            if(!this->isStatic()){
                this->argsCount++;
            }
            return this->argsCount;

        }

        u2 Method::getAccessFlags() const {
            return ClassMember::getAccessFlags();
        }

        void Method::setAccessFlags(u2 accessFlags) {
            ClassMember::setAccessFlags(accessFlags);
        }

        const string &Method::getName() const {
            return ClassMember::getName();
        }

        void Method::setName(const std::string &name) {
            ClassMember::setName(name);
        }

        const string &Method::getDescriptor() const {
            return ClassMember::getDescriptor();
        }

        void Method::setDescriptor(const string &descriptor) {
            ClassMember::setDescriptor(descriptor);
        }

        JavaClass *Method::getJavaClass() const {
            return ClassMember::getJavaClass();
        }

        void Method::setJavaClass(JavaClass *javaClass) {
            ClassMember::setJavaClass(javaClass);
        }

        bool Method::isPublic() {
            return ClassMember::isPublic();
        }

        bool Method::isFinal() {
            return ClassMember::isFinal();
        }

        bool Method::isStatic() {
            return ClassMember::isStatic();
        }

        bool Method::isInterface() {
            return ClassMember::isInterface();
        }

        bool Method::isSynthetic() {
            return ClassMember::isSynthetic();
        }

        bool Method::isAnnotation() {
            return ClassMember::isAnnotation();
        }

        bool Method::isEnum() {
            return ClassMember::isEnum();
        }

        FieldRef::FieldRef() {}

        FieldRef::~FieldRef() {

        }

        FieldInfo *FieldRef::resolvedFields() {
            if(this->fieldInfo==nullptr){
                this->resolveFieldRef();
            }
            return this->fieldInfo;
        }

        void FieldRef::resolveFieldRef() {
            JavaClass* javaClass=this->getConstantsPool()->getJavaClass();
            JavaClass* currentJavaClass=this->resolvedJavaClass();
            FieldInfo* fieldInfo=this->lookupField(currentJavaClass,this->getName(),this->getDescriptor());

            if(fieldInfo== nullptr){

            }
        //    dynamic_cast<ClassMember*>(this->fieldInfo)->setJavaClass(currentJavaClass);
         //   this->fieldInfo->setJavaClass(currentJavaClass);
            this->fieldInfo=fieldInfo;
        }

        FieldInfo *FieldRef::lookupField(JavaClass *javaClass, std::string name, std::string descriptor) {
            std::list<Heap::FieldInfo*> fieldInfoList=javaClass->getFieldInfoList();
            std::list<Heap::FieldInfo*>::iterator fieldInfoListIterator;
            for (fieldInfoListIterator=fieldInfoList.begin();fieldInfoListIterator!=fieldInfoList.end();fieldInfoListIterator++) {
                Heap::FieldInfo* fieldInfoIter=(*fieldInfoListIterator);
                if(fieldInfoIter->getName()==name&&fieldInfoIter->getDescriptor()==descriptor){
                    fieldInfoIter->setJavaClass(javaClass);
                    return fieldInfoIter;
                }
            }
            std::list<JavaClass*> javaClasses=javaClass->getInterfaces();
            if(javaClasses.size()>0){
                std::list<JavaClass*>::iterator jIterator;
                for (jIterator=javaClasses.begin(); jIterator!=javaClasses.end() ; ++jIterator) {
                    FieldInfo *fieldInfos=this->lookupField((*jIterator),name,descriptor);
                    if(fieldInfos!= nullptr){
                        fieldInfos->setJavaClass((*jIterator));
                        return fieldInfos;
                    }

                }
            }


            if(javaClass->getSuperClass()!= nullptr){
                return this->lookupField(javaClass->getSuperClass(),name,descriptor);
            }

            return nullptr;
        }

        FieldInfo *FieldRef::getFieldInfo() const {
            return fieldInfo;
        }

        void FieldRef::setFieldInfo(FieldInfo *fieldInfo) {
            FieldRef::fieldInfo = fieldInfo;
        }

        bool FieldRef::isAccessibleTo() {
            return false;
        }

        int FieldInfo::getSlotId() const {
            return slotId;
        }

        void FieldInfo::setSlotId(int slotId) {
            this->slotId = slotId;
        }

        bool FieldInfo::isLongOrDouble() {
            return this->getDescriptor()=="J"||this->getDescriptor()=="D";
        }

        int FieldInfo::getConstantValueIndex() const {
            return constantValueIndex;
        }

        void FieldInfo::setConstantValueIndex(int constantValueIndex) {
            FieldInfo::constantValueIndex = constantValueIndex;
        }

        u2 FieldInfo::getAccessFlags() const {
            return ClassMember::getAccessFlags();
        }

        void FieldInfo::setAccessFlags(u2 accessFlags) {
            ClassMember::setAccessFlags(accessFlags);
        }

        const string &FieldInfo::getName() const {
            return ClassMember::getName();
        }

        void FieldInfo::setName(const std::string &name) {
            ClassMember::setName(name);
        }

        const string &FieldInfo::getDescriptor() const {
            return ClassMember::getDescriptor();
        }

        void FieldInfo::setDescriptor(const string &descriptor) {
            ClassMember::setDescriptor(descriptor);
        }

        JavaClass *FieldInfo::getJavaClass() const {
            return ClassMember::getJavaClass();
        }

        void FieldInfo::setJavaClass(JavaClass *javaClass) {
            ClassMember::setJavaClass(javaClass);
        }

        bool FieldInfo::isPublic() {
            return ClassMember::isPublic();
        }

        bool FieldInfo::isFinal() {
            return ClassMember::isFinal();
        }

        bool FieldInfo::isStatic() {
            return ClassMember::isStatic();
        }

        bool FieldInfo::isInterface() {
            return ClassMember::isInterface();
        }

        bool FieldInfo::isAbstract() {
            return ClassMember::isAbstract();
        }

        bool FieldInfo::isSynthetic() {
            return ClassMember::isSynthetic();
        }

        bool FieldInfo::isAnnotation() {
            return ClassMember::isAnnotation();
        }

        bool FieldInfo::isEnum() {
            return ClassMember::isEnum();
        }


        SymRef::SymRef() {}

        SymRef::~SymRef() {

        }

        ClassRef::ClassRef() {}

        ClassRef::~ClassRef() {

        }

        const string &ClassRef::getClassName() const {
            return SymRef::getClassName();
        }

        void ClassRef::setClassName(const string &className) {
            SymRef::setClassName(className);
        }

        JavaClass *ClassRef::getJavaClass() const {
            return SymRef::getJavaClass();
        }

        void ClassRef::setJavaClass(JavaClass *javaClass) {
            SymRef::setJavaClass(javaClass);
        }

        JavaClass *ClassRef::resolvedJavaClass() {
            return SymRef::resolvedJavaClass();
        }

        JavaClass *ClassRef::resolveClassRef() {
            return SymRef::resolveClassRef();
        }

        bool ClassRef::isAccessiableTo(JavaClass *targetClass) {
            return SymRef::isAccessiableTo(targetClass);
        }


        RuntimeConstantsPool *SymRef::getConstantsPool() const {
            return constantsPool;
        }

        void SymRef::setConstantsPool(RuntimeConstantsPool *constantsPool) {
            SymRef::constantsPool = constantsPool;
        }

        const string &SymRef::getClassName() const {
            return className;
        }

        void SymRef::setClassName(const string &className) {
            SymRef::className = className;
        }

        JavaClass *SymRef::getJavaClass() const {
            return javaClass;
        }

        void SymRef::setJavaClass(JavaClass *javaClass) {
            SymRef::javaClass = javaClass;
        }

        JavaClass *SymRef::resolvedJavaClass() {
            if(this->javaClass== nullptr){
                this->resolveClassRef();
            }
            return this->javaClass;
        }

        JavaClass *SymRef::resolveClassRef() {
            JavaClass* d=this->getConstantsPool()->getJavaClass();
            JavaClass* c=d->getClassLoader()->loadClass(this->className);
            if(!c->isAccessibleTo(d)){
                cerr<<""<<endl;
            }
            this->setJavaClass(c);
            return c;
        }

        bool SymRef::isAccessiableTo(JavaClass* javaClass) {
            return true;
        }

        InterfaceMethodRef::InterfaceMethodRef() {}

        InterfaceMethodRef::~InterfaceMethodRef() {

        }



        Method *InterfaceMethodRef::getMethod() const {
            return method;
        }

        void InterfaceMethodRef::setMethod(Method *method) {
            InterfaceMethodRef::method = method;
        }

        void MethodRef::resolveMethodRef() {
            JavaClass* javaClass=this->getConstantsPool()->getJavaClass();
            JavaClass* thisClass=this->resolvedJavaClass();
            if(thisClass->isInterface()){

            }
            Heap::Method* method=this->lookupMethod(thisClass,this->getName(),this->getDescriptor());
            if(method== nullptr){

            }
            this->method=method;


        }

        Method *MethodRef::resolvedMethod() {
            if(this->method== nullptr){
                this->resolveMethodRef();
            }
            return this->method;
        }

        Method *MethodRef::lookupMethod(JavaClass *javaClass, std::string methodName, std::string descriptor) {
            Heap::Method* method=this->lookupMethodInClass(javaClass,methodName,descriptor);
            if(method== nullptr){
                method=this->lookupMethodInInterface(javaClass->getInterfaces(),methodName,descriptor);
            }
            return method;
        }

        Method *MethodRef::lookupMethodInClass(JavaClass *javaClass, std::string methodName, std::string descriptor) {


            std::list<Heap::Method*> methodList=javaClass->getMethodInfosList();
            std::list<Heap::Method*>::iterator methodIterator;
            for (methodIterator=methodList.begin(); methodIterator!=methodList.end() ; ++methodIterator) {
                Heap::Method* methodIter=(*methodIterator);

                if(methodIter->getName()==methodName&&(methodIter->getDescriptor()==descriptor)){
                    return (*methodIterator);
                }
            }

            JavaClass* superJavaClass=javaClass->getSuperClass();
            std::list<Heap::Method*> superMethodList=superJavaClass->getMethodInfosList();
            std::list<Heap::Method*>::iterator methodSuperIterator;
            for (methodSuperIterator=superMethodList.begin(); methodSuperIterator!=superMethodList.end() ; ++methodSuperIterator) {
                Heap::Method* methodIter=(*methodSuperIterator);

                if(methodIter->getName()==methodName&&(methodIter->getDescriptor()==descriptor)){
                    return (*methodSuperIterator);
                }
            }
            return nullptr;
        }

        Method *MethodRef::lookupMethodInInterface(std::list<JavaClass*> interfaceJavaClasses, std::string methodName, std::string descriptor) {

            std::list<JavaClass*>::iterator intIterator;

            for (intIterator=interfaceJavaClasses.begin(); intIterator!=interfaceJavaClasses.end(); ++intIterator) {
                std::list<Heap::Method*>::iterator methodIterator;
                JavaClass* interfaceJavaClass=(*intIterator);
                std::list<Heap::Method*> methodList=interfaceJavaClass->getMethodInfosList();
                for (methodIterator=methodList.begin();methodIterator!=methodList.end() ; ++methodIterator) {
                    if((*methodIterator)->getName()==methodName&&((*methodIterator)->getDescriptor()==descriptor)){
                        return (*methodIterator);
                    }
                }
                Heap::Method* resultMethod=this->lookupMethodInInterface(interfaceJavaClass->getInterfaces(),methodName,descriptor);
                if(resultMethod!= nullptr){
                    return resultMethod;
                }
            }

            return nullptr;
        }

        Method *InterfaceMethodRef::resolvedInterfaceMethod() {
            if(this->method== nullptr){
                this->resolveInterfaceMethodRef();
            }
            return this->method;
        }

        void InterfaceMethodRef::resolveInterfaceMethodRef() {

        }

        Method *InterfaceMethodRef::lookupInterfaceMethod(JavaClass *javaClass, std::string methodName,
                                                          std::string descriptor) {
            return nullptr;
        }
    }
  }