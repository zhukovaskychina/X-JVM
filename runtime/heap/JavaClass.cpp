//
// Created by zhukovasky on 2020/8/19.
//

#include "JavaClass.h"
#include "HeapAcessFlags.h"
#include "../../common/JvmEnums.h"
#include "../../classfile/Constant_Utf8_info.h"
#include "../../classfile/Constant_Class.h"
#include "../../classfile/Constant_MethodRef.h"
#include "../../classfile/Constant_NameAndType_info.h"
#include "../../classfile/Constant_FieldRef.h"
#include "../../classfile/Constant_InterfaceMethodRef.h"
#include "../../classfile/Constant_String_info.h"
#include "../../classfile/Constant_Integer_info.h"
#include "../../classfile/Constant_Float_info.h"
#include "../../classfile/attributeInfo/Code_attribute.h"
#include "../../classfile/attributeInfo/Exception_table.h"
#include "../../utils/StringUtils.h"
#include "../HeapConstantUtils.h"
#include "../detail/ArrayCoallocLayout.h"
#include <memory>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "../../utils/PlatformCompat.h"

namespace Runtime{

namespace {

size_t primitiveArrayElemBytes(const std::string& n) {
    if (n.size() < 2 || n[0] != '[') {
        return 0;
    }
    const char t = n[1];
    if (t == 'L' || t == '[') {
        return 0;
    }
    switch (t) {
    case 'Z':
    case 'B':
        return 1;
    case 'S':
    case 'C':
        return 2;
    case 'I':
    case 'F':
        return 4;
    case 'J':
    case 'D':
        return 8;
    default:
        return 0;
    }
}

} // namespace
    JavaClass::JavaClass(ClassFile::ClassFile *classFile) : classFile(classFile) {
        this->classFile=classFile;
        this->constantPoolsList=classFile->getConstantPools();
        this->accessFlags=classFile->getAccessFlags();
        this->superClassCount=classFile->getSuperClass();
        this->superClassName=classFile->getSuperClassName();
        this->parseThisClassName();
        this->instanceCount=classFile->getInterfaceCounts();
        this->interfacesNames=classFile->getInterfaceNames();
        this->parseMethods(classFile->getMethodInfoList());
        this->parseFields();
        ClassFile::ConstantPoolsList *constantPools=classFile->getConstantPools();
        this->initRuntimeConstantsPool(constantPools);
        this->parseInterfaces();
    }


    //获得主方法
    Runtime::Heap::Method *JavaClass::getMainMethod() {
        std::list<Heap::Method*>::iterator methodIterators;
        for (methodIterators=this->methodInfosList.begin(); methodIterators!=this->methodInfosList.end()  ; ++methodIterators) {
            Heap::Method* mainMethod=(*methodIterators);
            if(mainMethod->getName()=="main"&&mainMethod->getDescriptor()=="([Ljava/lang/String;)V"){
                return mainMethod;
            }
        }
        return nullptr;
    }

    bool JavaClass::isPublic() {
        return this->accessFlags&&Runtime::ClassEnum::ACC_PUBLIC;
    }

    bool JavaClass::isFinal() {
        return this->accessFlags&&Runtime::ClassEnum::ACC_FINAL;
    }

    bool JavaClass::isSuper() {
        return this->accessFlags&&Runtime::ClassEnum ::ACC_SUPER;
    }

    bool JavaClass::isInterface() {
        return this->accessFlags&&Runtime::ClassEnum ::ACC_INTERFACE;
    }

    bool JavaClass::isAbstract() {
        return this->accessFlags&&Runtime::ClassEnum ::ACC_ABSTRACT;
    }

    bool JavaClass::isSynthetic() {
        return this->accessFlags&&Runtime::ClassEnum ::ACC_SYNTHETIC;
    }

    bool JavaClass::isAnnotation() {
        return this->accessFlags&&Runtime::ClassEnum ::ACC_ANNOTATION;
    }

    bool JavaClass::isEnum() {
        return this->accessFlags&&Runtime::ClassEnum ::ACC_ENUM;
    }

    bool JavaClass::isArray() {
        return Utils::StringUtils::startsWith(this->getThisClassName(), "[");
    }

    bool JavaClass::isPrimitiveArray() const {
        const std::string& n = thisClassName;
        if (n.size() < 2 || n[0] != '[') {
            return false;
        }
        const char c = n[1];
        return c == 'Z' || c == 'B' || c == 'S' || c == 'C' || c == 'I' || c == 'J' || c == 'F' || c == 'D';
    }

    u2 JavaClass::getClassAccessFlags() const {
        return this->accessFlags;
    }

    void JavaClass::initRuntimeConstantsPool(ClassFile::ConstantPoolsList *constantPoolsList) {
        this->runtimeConstantsPool=new Runtime::Heap::RuntimeConstantsPool();
        this->runtimeConstantsPool->setJavaClass(this);
        int size=constantPoolsList->getSize();
        std::list<Runtime::Heap::ConstantsPoolObject*> constantPoolObjectList;
        std::vector<Runtime::Heap::ConstantsPoolObject*> constantPoolObjectVector;
        for(int j=1;j<size;j++){
            if(constantPoolsList->getIndex(j)== nullptr){
                constantPoolObjectVector.push_back(nullptr);
                continue;
            }
            u1 tag=constantPoolsList->getIndex(j)->getTag();
            ClassFile::ConstantPools *iter=constantPoolsList->getIndex(j);
            Runtime::Heap::ConstantsPoolObject *constantsPoolObject=new Runtime::Heap::ConstantsPoolObject();
            switch (tag){
                default:
                    constantsPoolObject= nullptr;
                    break;
                case CONSTANT_Class: {

                    ClassFile::Constant_Class *constantClass= reinterpret_cast<ClassFile::Constant_Class*>(iter);
                    u2 nameIndex=constantClass->getNameIndex();
                    Heap::ClassRef *classRef=new Heap::ClassRef();
                    std::string name=HeapConstantUtils::getUTF8(nameIndex,constantPoolsList);
                    classRef->setClassName(name);
                    classRef->setConstantsPool(this->runtimeConstantsPool);
                    classRef->setJavaClass(nullptr);
                //    classRef->resolveClassRef();
                    constantsPoolObject->setClassRef(reinterpret_cast<Heap::ClassRef*>(classRef));
                    constantsPoolObject->setConstantType("class");

                    break;
                }
                case CONSTANT_MethodRef: {
                    ClassFile::Constant_MethodRef *constantMethod= reinterpret_cast<ClassFile::Constant_MethodRef*>(iter);

                    u2 classIndex=constantMethod->getClassIndex();
                    u2 nameAndTypeIndex=constantMethod->getNameAndTypeIndex();

                    Heap::MemberRef *methodRef=new Heap::MethodRef();
                    reinterpret_cast<Heap::MethodRef*>(methodRef)->parseMethods(classFile);

                    methodRef->setName(this->getFieldOrMethodName(nameAndTypeIndex,constantPoolsList));
                    methodRef->setDescriptor(this->getDescriptor(nameAndTypeIndex,constantPoolsList));
                    methodRef->setClassName(this->getClassName(classIndex,constantPoolsList));
                    methodRef->setConstantsPool(this->runtimeConstantsPool);

                    Heap::MethodRef* methodRefs= reinterpret_cast<Heap::MethodRef*>(methodRef);
                    methodRefs->setMethod(nullptr);
                    constantsPoolObject->setMethodRef(methodRefs);
                    constantsPoolObject->setConstantType("method");

                    break;
                }
                case CONSTANT_FieldRef: {
                    ClassFile::Constant_FieldRef *constantFieldRef= reinterpret_cast<ClassFile::Constant_FieldRef*>(iter);

                    u2 classIndex=constantFieldRef->getClassIndex();
                    u2 nameAndTypeIndex=constantFieldRef->getNameAndTypeIndex();
                    Heap::FieldRef *fieldRef=new Heap::FieldRef();
                    fieldRef->setConstantsPool(this->runtimeConstantsPool);


                    fieldRef->setClassName(this->getClassName(classIndex,constantPoolsList));
                   // fieldRef->setJavaClass(this->getClassLoader()->loadClass(fieldRef->getClassName()));
                    fieldRef->setJavaClass(nullptr);
                    dynamic_cast<Heap::FieldRef*>(fieldRef)->setName(this->getFieldOrMethodName(nameAndTypeIndex,constantPoolsList));
                    dynamic_cast<Heap::FieldRef*>(fieldRef)->setDescriptor(this->getDescriptor(nameAndTypeIndex,constantPoolsList));
                    dynamic_cast<Heap::FieldRef*>(fieldRef)->setFieldInfo(nullptr);

                    constantsPoolObject->setFieldRef(dynamic_cast<Heap::FieldRef*>(fieldRef));
                    //       constantsPoolObject->setConstantPoolObjectValue(dynamic_cast<Heap::FieldRef*>(fieldRef));
                    constantsPoolObject->setConstantType("field");
                    break;
                }
                case CONSTANT_InterfaceMethodRef: {
                    ClassFile::Constant_InterfaceMethodRef *constantInterfaceMethodRef= reinterpret_cast<ClassFile::Constant_InterfaceMethodRef*>(iter);

                    u2 classIndex=constantInterfaceMethodRef->getClassIndex();
                    u2 nameAndTypeIndex=constantInterfaceMethodRef->getNameAndTypeIndex();
                    Heap::InterfaceMethodRef *interfaceMethodRef=new Heap::InterfaceMethodRef();
                    interfaceMethodRef->setConstantsPool(this->runtimeConstantsPool);
                    interfaceMethodRef->setClassName(this->getClassName(classIndex,constantPoolsList));
                    interfaceMethodRef->setName(this->getFieldOrMethodName(nameAndTypeIndex,constantPoolsList));
                    interfaceMethodRef->setDescriptor(this->getDescriptor(nameAndTypeIndex,constantPoolsList));
                    interfaceMethodRef->setMethod(nullptr);

                    constantsPoolObject->setInterfaceMethodRef(dynamic_cast<Heap::InterfaceMethodRef*>(interfaceMethodRef));

                    constantsPoolObject->setConstantType("interfaceMethod");
                    break;
                }
                case CONSTANT_String: {
                    ClassFile::Constant_StringInfo *constantStringInfo= reinterpret_cast<ClassFile::Constant_StringInfo*>(iter);

                    u2 stringIndex=constantStringInfo->getStringIndex();
                    constantsPoolObject->setConstantType("string");

                    constantsPoolObject->setStringValue(this->getUTF8(stringIndex,constantPoolsList));
                    break;
                }
                case CONSTANT_Integer: {
                    ClassFile::Constant_IntegerInfo *constantIntegerInfo= reinterpret_cast<ClassFile::Constant_IntegerInfo*>(iter);
                    constantsPoolObject->setConstantType("int");
                    constantsPoolObject->setIntValue(constantIntegerInfo->getBytes());
                    break;
                }
                case CONSTANT_Float: {
                    ClassFile::Constant_FloatInfo *constantIntegerInfo= reinterpret_cast<ClassFile::Constant_FloatInfo*>(iter);
                    constantsPoolObject->setConstantType("float");
                    constantsPoolObject->setIntValue(constantIntegerInfo->getBytes());
                    break;
                }
                case CONSTANT_Long: {
                    ClassFile::Constant_IntegerInfo *constantIntegerInfo= reinterpret_cast<ClassFile::Constant_IntegerInfo*>(iter);
                    constantsPoolObject->setConstantType("int");
                    constantsPoolObject->setIntValue(constantIntegerInfo->getBytes());
                    break;
                }
                case CONSTANT_Double: {
                    ClassFile::Constant_IntegerInfo *constantIntegerInfo= reinterpret_cast<ClassFile::Constant_IntegerInfo*>(iter);
                    constantsPoolObject->setConstantType("int");
                    constantsPoolObject->setIntValue(constantIntegerInfo->getBytes());
                    break;
                };

            }
            constantPoolObjectList.push_back(constantsPoolObject);
            constantPoolObjectVector.push_back(constantsPoolObject);
        }
        this->runtimeConstantsPool->setJavaClass(this);
        this->runtimeConstantsPool->setConstantsPoolObjectVector(constantPoolObjectVector);
    }


    std::vector<Slots*> JavaClass::getStaticVars() const {
        return staticVars;
    }

    void JavaClass::setStaticVars(std::vector<Slots*> staticVars) {
        this->staticVars = staticVars;
    }


    const list<Heap::Method *> &JavaClass::getMethodInfosList() const {
        return methodInfosList;
    }

    const list<Heap::FieldInfo *> &JavaClass::getFieldInfoList() const {
        return fieldInfoList;
    }


    void JavaClass::parseFields() {
        
        this->fieldInfoList.clear();
        
        int fieldCount=classFile->getFieldsCount();

        if(fieldCount>0){
            std::list<ClassFile::FieldInfo*> clsFieldInfoList=classFile->getFieldInfoList();
            std::list<ClassFile::FieldInfo*>::iterator fieldInfoIters;
            for (fieldInfoIters=clsFieldInfoList.begin();fieldInfoIters!=clsFieldInfoList.end(); ++fieldInfoIters) {
                ClassFile::FieldInfo* fieldInfoIterator=(*fieldInfoIters);
                fieldInfoIterator->getNameIndex();
                Heap::FieldInfo* fieldInfo=new Heap::FieldInfo();
                fieldInfo->setAccessFlags(fieldInfoIterator->getAccessFlags());

                fieldInfo->setName(this->getUTF8(fieldInfoIterator->getNameIndex(),this->constantPoolsList));
                fieldInfo->setDescriptor(this->getUTF8(fieldInfoIterator->getDescriptorIndex(),this->constantPoolsList));
                //fieldInfo->setJavaClass(this);

                //fieldInfo->setSlotId(0);
                ClassFile::ConstantValue *constantValue=this->getConstantValueAttribute(fieldInfoIterator);
                if(constantValue!= nullptr){
                    fieldInfo->setConstantValueIndex(constantValue->getConstantvalueIndex());
                }


                this->fieldInfoList.push_back(fieldInfo);
            }
        }
    }

    std::list<Runtime::Heap::Method*> JavaClass::getStaticMethod() {

        std::list<Heap::Method*>::iterator methodIterator;
        for (methodIterator=this->methodInfosList.begin(); methodIterator!=this->methodInfosList.end(); ++methodIterator) {
            Heap::Method* method=(*methodIterator);
            if(method->isStatic()){
                this->staticMethodsList.push_back(method);
            }
        }
        return this->staticMethodsList;
    }






    std::string JavaClass::getUTF8(u2 index, ClassFile::ConstantPoolsList *classFileConstantsPools) {

        ClassFile::Constant_Utf8Info *constantUtf8Info= reinterpret_cast<ClassFile::Constant_Utf8Info*>(classFileConstantsPools->getIndex(index));

        std::string result=constantUtf8Info->toString();
        return std::string(result);
    }

    std::string JavaClass::getClassName(u2 index, ClassFile::ConstantPoolsList *constantPools) {
        ClassFile::Constant_Class *constantClass= reinterpret_cast<ClassFile::Constant_Class*>(constantPools->getIndex(index));
        u2 nameIndex=constantClass->getNameIndex();
        return this->getUTF8(nameIndex,constantPools);
    }

    std::string JavaClass::getDescriptor(u2 index, ClassFile::ConstantPoolsList *constantPools) {
        ClassFile::Constant_NameAndTypeInfo *constantNameAndTypeInfo= reinterpret_cast<ClassFile::Constant_NameAndTypeInfo*>(constantPools->getIndex(index));
        u2 descriptorIndex=constantNameAndTypeInfo->getDescriptorIndex();
        return this->getUTF8(descriptorIndex,constantPools);

    }

    std::string JavaClass::getFieldOrMethodName(u2 index, ClassFile::ConstantPoolsList *pConstantPools) {
        ClassFile::Constant_NameAndTypeInfo *constantNameAndTypeInfo= reinterpret_cast<ClassFile::Constant_NameAndTypeInfo*>(pConstantPools->getIndex(index));
        u2 nameIndex=constantNameAndTypeInfo->getNameIndex();
        return this->getUTF8(nameIndex,pConstantPools);
    }


    ClassLoader *JavaClass::getClassLoader() const {
        return classLoader;
    }

    void JavaClass::setClassLoader(ClassLoader *classLoader) {
        JavaClass::classLoader = classLoader;
    }

    std::string JavaClass::getThisClassName() {

        return this->javaClassName;
    }

    JavaClass *JavaClass::getSuperClass() const {
        if(this->superClassCount>0){
            return superClass;
        }
        return nullptr;
    }

    void JavaClass::setSuperClass(JavaClass *superClass) {
        JavaClass::superClass = superClass;
    }

    const string &JavaClass::getSuperClassName() const {
        return superClassName;
    }



    const vector<std::string> &JavaClass::getInterfacesNames() const {
        return interfacesNames;
    }

    const list<JavaClass *> JavaClass::getInterfaces() const {
        std::list<JavaClass*> interfacesList;

        for (int i = 0; i < this->interfacesNames.size(); ++i) {
            std::string interfaceName=this->interfacesNames[i];
            JavaClass* interfaceClass=this->getClassLoader()->loadClass(interfaceName);
            interfacesList.push_back(interfaceClass);
        }

        return interfacesList;
    }


    u1 JavaClass::getInstanceCount() const {
        return instanceCount;
    }

    void JavaClass::setInstanceCount(u1 instanceCount) {
        JavaClass::instanceCount = instanceCount;
    }

    u1 JavaClass::getStaticCount() const {
        return staticCount;
    }

    void JavaClass::setStaticCount(u1 staticCount) {
        JavaClass::staticCount = staticCount;
    }


    Heap::RuntimeConstantsPool *JavaClass::getRuntimeConstantsPool() const {
        return runtimeConstantsPool;
    }


    bool JavaClass::isAccessibleTo(JavaClass *target) {
        return this->isPublic();
    }

    std::string JavaClass::getPackageName() {
        this->getThisClassName();
        return std::string();
    }
    //创建类实例
    Object *JavaClass::createNewJavaObjectInstance() {
        Object *object=new Object(this);
        object->setJavaClass(this);
        const u1 n = this->instanceCount;
        if (n > 0) {
            Slots* arr = new Slots[static_cast<size_t>(n)]();
            object->setFields(arr);
        }
        return object;
    }

    void JavaClass::setInterfaces(const list<JavaClass *> &interfaces) {
        this->interfaces = interfaces;
    }

    void JavaClass::parseMethods(std::list<ClassFile::MethodInfo *> methodInfoList) {
       // this->methodInfosList.clear();
        std::list<ClassFile::MethodInfo*>::iterator iters;
        for (iters=methodInfoList.begin(); iters!=methodInfoList.end() ; ++iters) {
            ClassFile::MethodInfo *clsMethod=(*iters);

            Heap::Method* method=new Heap::Method();
            method->setName(this->getUTF8(clsMethod->getNameIndex(),this->constantPoolsList));
            u2 index=clsMethod->getDescriptorIndex();
            method->setDescriptor(this->getUTF8(index,this->constantPoolsList));
            method->setAccessFlags(clsMethod->getAccessFlags());
            method->setJavaClass(this);
            method->calcArgsSlotCount(method->getDescriptor());
            ClassFile::Code_attribute *codeAttribute=getCodeAttrs(clsMethod);
            if(codeAttribute!= nullptr){
                method->setCode(codeAttribute->getCode());
                method->setMaxStack(codeAttribute->getMaxStack());
                method->setMaxLocal(codeAttribute->getMaxLocals());
                method->clearExceptionHandlers();
                for (Exception_table *et : codeAttribute->getExceptionTableList()) {
                    if (!et) {
                        continue;
                    }
                    method->addExceptionHandler({et->getStartPc(), et->getEndPc(), et->getHandlerPc(),
                                               et->getCatchType()});
                }
            }

            this->methodInfosList.push_back(method);
        }


    }

    ClassFile::Code_attribute* JavaClass::getCodeAttrs(const ClassFile::MethodInfo *clsMethod) const {
        list<ClassFile::AttributesInfo*> attrList=clsMethod->getAttributesInfoList();
        list<ClassFile::AttributesInfo*>::iterator itersAttr;
        for (itersAttr=attrList.begin(); itersAttr!=attrList.end() ; ++itersAttr) {
            ClassFile::AttributesInfo *attributesInfoIter=(*itersAttr);
            if(attributesInfoIter->getTags()=="Code"){
                ClassFile::Code_attribute* codeAttribute= reinterpret_cast<ClassFile::Code_attribute*>(attributesInfoIter);
                return codeAttribute;
            }

        }
        return nullptr;
    }

    ClassFile::ConstantValue *JavaClass::getConstantValueAttribute(const ClassFile::FieldInfo *fieldInfo) const {
        list<ClassFile::AttributesInfo*> attrList=fieldInfo->getAttributesInfoList();
        list<ClassFile::AttributesInfo*>::iterator itersAttr;
        for (itersAttr=attrList.begin(); itersAttr!=attrList.end() ; ++itersAttr) {
            ClassFile::AttributesInfo *attributesInfoIter=(*itersAttr);
            if(attributesInfoIter->getTags()=="ConstantValue"){
                ClassFile::ConstantValue* codeAttribute= reinterpret_cast<ClassFile::ConstantValue*>(attributesInfoIter);
                return codeAttribute;
            }

        }
        return nullptr;
    }

    Object *JavaClass::createArray(int length) {
        if (length < 0) {
            length = 0;
        }
        const size_t primElem = primitiveArrayElemBytes(thisClassName);
        const bool isRefArray = !thisClassName.empty() && thisClassName[0] == '[' &&
                                thisClassName.size() >= 2 &&
                                (thisClassName[1] == 'L' || thisClassName[1] == '[');
        size_t body = 0;
        if (primElem > 0) {
            body = primElem * static_cast<size_t>(length);
        } else if (isRefArray) {
            body = sizeof(Object*) * static_cast<size_t>(length);
        } else {
            Object* object = new Object(this);
            object->setObjectType(thisClassName);
            object->setArrayLength(length);
            object->setData(nullptr);
            return object;
        }

        const size_t payOff = ArrayCoalloc::payloadOffset();
        const size_t total = payOff + body;
        auto* raw = static_cast<unsigned char*>(std::malloc(total));
        if (!raw) {
            Object* object = new Object(this);
            object->setObjectType(thisClassName);
            object->setArrayLength(length);
            object->setArrayCoallocated(false);
            if (primElem > 0) {
                if (length == 0) {
                    object->setData(nullptr);
                } else {
                    const size_t bytes = primElem * static_cast<size_t>(length);
                    object->setData(new uint8_t[bytes]());
                }
            } else if (length == 0) {
                object->setData(nullptr);
            } else {
                object->setData(new Object*[static_cast<size_t>(length)]());
            }
            return object;
        }
        std::memset(raw, 0, total);
        *reinterpret_cast<uint64_t*>(raw) = ArrayCoalloc::kMagic;
        auto* object = new (raw + ArrayCoalloc::objectOffset()) Object(this);
        object->setObjectType(thisClassName);
        object->setArrayLength(length);
        object->setArrayCoallocated(true);
        object->setData(raw + payOff);
        return object;
    }

    JavaClass::JavaClass(std::string className,ClassLoader* classLoader,bool initStarted) {
        this->thisClassName=className;
        this->classLoader=classLoader;
        this->superClass=this->classLoader->loadClass("java/lang/Object");
        this->accessFlags=ClassEnum::ACC_PUBLIC;
        this->initClassStarted=initStarted;
        this->initClassCompleted=initStarted;
        this->interfaces.push_back(this->classLoader->loadClass("java/io/Serializable"));
        this->interfaces.push_back(this->classLoader->loadClass("java/lang/Cloneable"));
    }

    void JavaClass::parseInterfaces() {
        std::list<JavaClass*> interfacesList;

        for (int i = 0; i < this->interfacesNames.size(); ++i) {
            std::string interfaceName=this->interfacesNames[i];
            ClassLoader* classLoader=this->getClassLoader();
         //   JavaClass* interfaceClass=classLoader->loadClass(interfaceName);
         //   interfacesList.push_back(interfaceClass);
        }

        this->interfaces=interfacesList;
    }

    void JavaClass::parseThisClassName() {
        this->javaClassName=this->getClassName(this->classFile->getThisClass(),
                                               (this->classFile->getConstantPools()));
    }

    bool JavaClass::initStarted() {

        return this->initClassStarted;
    }

    bool JavaClass::initCompleted() const {
        return this->initClassCompleted;
    }

    void JavaClass::startInit() {
        this->initClassStarted=true;
    }

    void JavaClass::markInitCompleted() {
        this->initClassCompleted = true;
    }

    void JavaClass::initSuperClass(JavaThread *javaThread, JavaClass* javaClass) {
        if(!javaClass->isInterface()){
            JavaClass* superClass=javaClass->getSuperClass();
            if(superClass!= nullptr&&superClass->initStarted()== false){
                this->initJavaClass(javaThread,superClass);
            }
        }
    }

    void JavaClass::scheduleClinit(JavaThread *javaThread, JavaClass *javaClass) {
        Heap::Method* clinitMethod=this->getClinitMethod();
        if(clinitMethod!= nullptr){
            JavaFrame* newFrame=new JavaFrame(javaThread,clinitMethod);
            javaThread->pushJavaFrame(newFrame);
        }

    }

    void JavaClass::initJavaClass(JavaThread* javaThread,JavaClass* javaClass) {
        this->startInit();
        this->scheduleClinit(javaThread,javaClass);
        this->initSuperClass(javaThread,javaClass);
        if (this->getClinitMethod() == nullptr) {
            this->markInitCompleted();
        }
    }

    Heap::Method *JavaClass::getClinitMethod() {

        return this->getStaticMethod("<clinit>","()V");
    }

    Heap::Method *JavaClass::getStaticMethod(std::string name, std::string descriptor) {
        std::list<Heap::Method*>::iterator methodIter;
        for (methodIter=this->staticMethodsList.begin(); methodIter!=this->staticMethodsList.end() ; ++methodIter) {
            Heap::Method* currentMethod=(*methodIter);
            std::string currentName=currentMethod->getName();
            std::string currentDescriptor=currentMethod->getDescriptor();
            if(currentName==name&&currentDescriptor==descriptor){
                return currentMethod;
            }
        }
        return nullptr;
    }

    std::string JavaClass::getString() {
        std::string result = "JavaClass{" + thisClassName + "}";
        return result;
    }

    std::string JavaClass::getConstantPoolString(u2 index) {
#ifdef _MSC_VER
        return std::string();
#else
        return std::string();
#endif
    }

}

