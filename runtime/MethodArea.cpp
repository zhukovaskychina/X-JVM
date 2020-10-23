//
// Created by zhukovasky on 2020/8/18.
//

#include "MethodArea.h"
#include "ClassLoader.h"
#include "../utils/StringUtils.h"
#include "heap/JavaClass.h"
#include "heap/StringConstantPools.h"

namespace Runtime{
    JavaClass *Runtime::MethodArea::loadJavaClass(std::string javaClassName) {
        JavaClass *javaClass=this->classTable[javaClassName];
        if(javaClass!= nullptr){
            return javaClass;
        }
        if(Utils::StringUtils::startsWith(javaClassName,"[")){
            return this->loadArrayClass(javaClassName);
        }
        return this->loadNonArrayClass(javaClassName);
    }

    void MethodArea::loadPrimitiveJavaClass(std::string javaClassName) {

    }

    void MethodArea::loadBasicJavaClass(std::string javaClassName) {

    }

    JavaClass *MethodArea::loadClass(std::string className) {
        JavaClass *javaClass=this->classTable[className];
        if(javaClass!= nullptr){
            return javaClass;
        }
        if(Utils::StringUtils::startsWith(className,"[")){
            return this->loadArrayClass(className);
        }
        return this->loadNonArrayClass(className);
    }

    JavaClass *MethodArea::loadNonArrayClass(std::string className) {
        Heap::Entry *entry=this->readClass(className);
        JavaClass* javaClass=this->defineClass(entry->getByteQueue());
        this->link(javaClass);
        return javaClass;
    }

    JavaClass *MethodArea::loadArrayClass(std::string className) {

        return nullptr;
    }

    Heap::Entry *MethodArea::readClass(std::string className) {
        return this->classPath->readClass(className);
    }

    JavaClass *MethodArea::defineClass(Lang::ByteQueue *data) {
        ClassFile::ClassFile *classFile=new ClassFile::ClassFile(data);
        classFile->readClassFile();
        Runtime::JavaClass *javaClass=new Runtime::JavaClass(classFile);
        resolveSuperClass(javaClass);
        resolveInterfaces(javaClass);
        this->classTable[javaClass->getThisClassName()]=javaClass;
        return javaClass;
    }

    void MethodArea::resolveSuperClass(Runtime::JavaClass *javaClass) {
        std::string thisClassName=javaClass->getThisClassName();
        //判断是否是java.lang.Object,实现双亲委派模型
        if(thisClassName!="java/lang/Object"){
            javaClass->setSuperClass(this->loadClass(javaClass->getSuperClassName()));
        }
    }

    void MethodArea::link(JavaClass *javaClass) {
        this->prepare(javaClass);
    }

    void MethodArea::resolveInterfaces(JavaClass *javaClass) {
        std::vector<string> interfacesName=javaClass->getInterfacesNames();
        std::list<JavaClass*> interfacesList;
        int interfacesCount= interfacesName.size();
        if(interfacesCount>0){
            JavaClass* interfacesClass;
            for (int i = 0; i < interfacesCount; ++i) {
                interfacesClass=this->loadClass(interfacesName[i]);
                interfacesList.push_back(interfacesClass);
            }
            javaClass->setInterfaces(interfacesList);
        }
    }

    void MethodArea::prepare(JavaClass *javaClass) {
        calculateInstanceFieldSlotsId(javaClass);
        calculateStaticFieldSlotsId(javaClass);
        allocateAndInitStaticVars(javaClass);
    }

    MethodArea::MethodArea() {
        this->initBootStrapClassLoader("");
        //加载java.lang中的各种class
        this->loadBasicJavaClass("");
        this->loadPrimitiveAllJavaClasses();
    }

    void MethodArea::loadPrimitiveAllJavaClasses() {

    }

    void MethodArea::initBootStrapClassLoader(std::string jreOptions) {
        this->classPath->parseBootAndExtClassPath(jreOptions);
    }

    void MethodArea::calculateInstanceFieldSlotsId(JavaClass *javaClass) {
        u1 slotId=0;
        if(javaClass->getSuperClass()!= nullptr){
            slotId=javaClass->getSuperClass()->getInstanceCount();
        }

        std::list<Heap::FieldInfo*> fieldInfoList=javaClass->getFieldInfoList();
        std::list<Heap::FieldInfo*>::iterator fieldInfoListIterator;
        for (fieldInfoListIterator=fieldInfoList.begin();fieldInfoListIterator!=fieldInfoList.end();fieldInfoListIterator++) {
            Heap::FieldInfo* fieldInfoIter=(*fieldInfoListIterator);
            if(!fieldInfoIter->isStatic()){
                fieldInfoIter->setSlotId(slotId);
                slotId++;
                if(fieldInfoIter->isLongOrDouble()){
                    slotId++;
                }
            }
        }

        javaClass->setInstanceCount(slotId);
    }

    void MethodArea::calculateStaticFieldSlotsId(JavaClass *javaClass) {

        u1 slotId=0;
        //计算java Field实例个数
        std::list<Heap::FieldInfo*> fieldInfoList=javaClass->getFieldInfoList();
        std::list<Heap::FieldInfo*>::iterator fieldInfoListIterator;
        for (fieldInfoListIterator=fieldInfoList.begin();fieldInfoListIterator!=fieldInfoList.end();fieldInfoListIterator++) {
            Heap::FieldInfo* fieldInfoIter=(*fieldInfoListIterator);
            if(fieldInfoIter->isStatic()){
                fieldInfoIter->setSlotId(slotId);
                slotId++;
                if(fieldInfoIter->isLongOrDouble()){
                    slotId++;
                }
            }
        }

        javaClass->setStaticCount(int(slotId));
    }

    void MethodArea::allocateAndInitStaticVars(JavaClass *javaClass) {
        u1 slotId=0;
        if(javaClass->getSuperClass()!= nullptr){
            slotId=javaClass->getSuperClass()->getInstanceCount();
        }

        int size=javaClass->getStaticCount();
        std::vector<Slots*> staticSlots;
        for (int i = 0; i < size; ++i) {
            staticSlots.push_back(nullptr);

        }
        javaClass->setStaticVars(staticSlots);
        //计算java Field实例个数
        std::list<Heap::FieldInfo*> fieldInfoList=javaClass->getFieldInfoList();
        std::list<Heap::FieldInfo*>::iterator fieldInfoListIterator;
        for (fieldInfoListIterator=fieldInfoList.begin();fieldInfoListIterator!=fieldInfoList.end();fieldInfoListIterator++) {
            Heap::FieldInfo* fieldInfoIter=(*fieldInfoListIterator);
            if(fieldInfoIter->isStatic()&&fieldInfoIter->isFinal()){
                Heap::FieldInfo *fieldInfo=fieldInfoIter;
                std::vector<Slots*> slotsVector=javaClass->getStaticVars();
                Runtime::Heap::RuntimeConstantsPool *runtimeConstantsPool=javaClass->getRuntimeConstantsPool();
                //   cout<<fieldInfo->getName()<<endl;
                u1 constantValueIndex=fieldInfo->getConstantValueIndex();
                u1 slotId=fieldInfo->getSlotId();
                if(constantValueIndex>0){
                    std::string fieldDescriptor=fieldInfo->getDescriptor();
                    //cout<<"fieldDescriptor"<<fieldDescriptor<<endl;
                    Slots *slots=new Slots();
                    //FieldType 类型
                    //B byte,C char,D double,F float,I int,J long,L className 引用类型 ,S short,Z bool,[ reference
                    if(fieldDescriptor=="Z"||fieldDescriptor=="B"||fieldDescriptor=="C"||fieldDescriptor=="S"||fieldDescriptor=="I"){

                        int value=runtimeConstantsPool->getIndex(constantValueIndex)->getIntValue();
                        slots->setNums(value);

                    }
                    if(fieldDescriptor=="J"){
                        long value=runtimeConstantsPool->getIndex(constantValueIndex)->getLongValue();
                        slots->setNums(value);
                    }
                    if(fieldDescriptor=="F"){
                        float value=runtimeConstantsPool->getIndex(constantValueIndex)->getFloatValue();
                        slots->setFloatValue(value);
                    }
                    if(fieldDescriptor=="D"){
                        double value=runtimeConstantsPool->getIndex(constantValueIndex)->getDoubleValue();
                        slots->setDoubleValue(value);
                    }
                    if(fieldDescriptor=="Ljava/lang/String"){

                        string str=runtimeConstantsPool->getIndex(constantValueIndex)->getStringValue();

                        Object* obj=Runtime::Heap::JString::getJString()->getJString(javaClass->getClassLoader(),str);
                        javaClass->getStaticVars()[slotId]->setRefs(obj);
                    }
                    // javaClass->getStaticVars().push_back(slots);
                }

            }
        }
        javaClass->setInstanceCount(slotId);
    }


}
