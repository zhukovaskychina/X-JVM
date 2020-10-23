//
// Created by zhukovasky on 2020/8/13.
//

#include "ClassLoader.h"
#include "../utils/FileUtils.h"
#include "heap/JavaClass.h"
#include "heap/ClassMember.h"
#include "heap/StringConstantPools.h"

namespace Runtime{
    class Heap::FieldInfo;

    ClassPath::ClassPath() {
       // this->parseBootAndExtClassPath("");
    }

    ClassPath::~ClassPath() {

    }

    Heap::Entry *ClassPath::readClass(std::string className) {
        Heap::Entry *entry;
        std::list<Heap::Entry*>::iterator entryIter;
        std::string javaClassName="";
        if(!Utils::StringUtils::endsWith(className,".class")){
            javaClassName=className+".class";
        }else{
            javaClassName=className;
        }


        for (entryIter=this->bootClassPathEntryList.begin();entryIter!=this->bootClassPathEntryList.end(); ++entryIter) {
            Heap::Entry* zipEntry=*entryIter;
            Heap::Entry* temp=zipEntry->readClass(javaClassName);
            if(temp!= nullptr){
                return temp;
            }
        }

        for (entryIter=this->extClassPathEntryList.begin();entryIter!=this->extClassPathEntryList.end(); ++entryIter) {
            Heap::Entry* zipEntry=*entryIter;
            Heap::Entry* temp=zipEntry->readClass(javaClassName);
            if(temp!= nullptr){
                return temp;
            }
        }

        for (entryIter=this->userClassPathEntryList.begin();entryIter!=this->userClassPathEntryList.end(); ++entryIter) {
            Heap::Entry* zipEntry=*entryIter;
            Heap::Entry* temp=zipEntry->readClass(javaClassName);
            if(temp!= nullptr){
                return temp;
            }
        }
        return nullptr;
    }

    ClassPath *ClassPath::parse(std::string jreOptions,std::string cpOptions) {
        ClassPath *classPath=new ClassPath();
        classPath->parseBootAndExtClassPath(jreOptions);
        classPath->parseUserClassPath(cpOptions);
        return classPath;
    }

    ClassPath *ClassPath::parseBootAndExtClassPath(std::string jreOptions) {

        std::string jreHome;
        if(jreOptions.empty()){
            jreHome=this->getJreHome();
        } else{
            jreHome=jreOptions;
        }
        std::string jreListPath=jreHome+"/lib/";
        this->bootClassPathEntryList=this->getWildCardEntry(jreListPath);
        //获取Ext下的libs
        std::string jreExtListPath=jreHome+"/lib/ext/";
        this->extClassPathEntryList=this->getWildCardEntry(jreExtListPath);
        return this;
    }

    ClassPath *ClassPath::parseUserClassPath(std::string cpOption) {
        Heap::Entry *entry=Heap::Entry::newEntry(cpOption);
        this->userClassPathEntryList.push_back(entry);

        return this;
    }

    const list<Heap::Entry *> &ClassPath::getBootClassPathEntryList() const {
        return bootClassPathEntryList;
    }


    const list<Heap::Entry *> &ClassPath::getExtClassPathEntryList() const {
        return extClassPathEntryList;
    }



    const list<Heap::Entry *> &ClassPath::getUserClassPathEntryList() const {
        return userClassPathEntryList;
    }




    std::list<Heap::Entry*> ClassPath::getWildCardEntry(std::string path) {
        std::list<std::string> fileList;
        std::list<Heap::Entry*> entryList;
        Utils::FileUtils::listJarOrZipFile(path,fileList,"jar");
        list<std::string>::iterator iter;
        for(iter=fileList.begin();iter!=fileList.end();iter++){
            Heap::Entry *zipEntry=new Heap::ZipEntry(path+*iter);
            entryList.push_back(zipEntry);
        }
        return entryList;
    }


    std::string ClassPath::getJreHome() {
        char *javaHome = nullptr;
        javaHome = getenv("JAVA_HOME");
        if(javaHome==nullptr){
            exit(0);
        }
        return javaHome;
    }




    ClassLoader::ClassLoader(ClassPath *classPath) : classPath(classPath) {
        this->classPath=classPath;
    }

    ClassLoader *ClassLoader::getBootstrapClassLoader(std::string jreOptions) {
        this->initBootStrapClassLoader(jreOptions);
        return nullptr;
    }

    void ClassLoader::initBootStrapClassLoader(std::string jreOptions) {
        this->classPath->parseBootAndExtClassPath(jreOptions);
    }

    JavaClass *ClassLoader::loadClass(std::string className) {
        JavaClass *javaClass=this->classMap[className];
        if(javaClass!= nullptr){
            return javaClass;
        }
        if(Utils::StringUtils::startsWith(className,"[")){
            return this->loadArrayClass(className);
        }
        return this->loadNonArrayClass(className);
    }

    JavaClass *ClassLoader::loadNonArrayClass(std::string className) {
        Heap::Entry *entry=this->readClass(className);
        cout<<className<<endl;
        JavaClass* javaClass=this->defineClass(entry->getByteQueue());
        this->link(javaClass);
        return javaClass;
    }

    Heap::Entry* ClassLoader::readClass(std::string className) {
       //  this->classPath->readClass(className)->getBootClassPath()->getEntry();
        return this->classPath->readClass(className);
    }

    void ClassLoader::link(JavaClass *javaClass) {
        this->prepare(javaClass);
    }

    JavaClass* ClassLoader::defineClass(ClassFile::ByteQueue *data) {
        ClassFile::ClassFile *classFile=new ClassFile::ClassFile(data);
        classFile->readClassFile();
        Runtime::JavaClass *javaClass=new Runtime::JavaClass(classFile);
        javaClass->setClassLoader(this);
        resolveSuperClass(javaClass);
        resolveInterfaces(javaClass);
        this->classMap[javaClass->getThisClassName()]=javaClass;
        return javaClass;
    }


    JavaClass *ClassLoader::parseClass(u1 *data) {
        return nullptr;
    }

    void ClassLoader::resolveSuperClass(Runtime::JavaClass* javaClass) {
        std::string thisClassName=javaClass->getThisClassName();
        //判断是否是java.lang.Object,实现双亲委派模型
        if(thisClassName!="java/lang/Object"){
            javaClass->setSuperClass(javaClass->getClassLoader()->loadClass(javaClass->getSuperClassName()));
        }

    }

    void ClassLoader::resolveInterfaces(JavaClass* javaClass) {
        std::vector<string> interfacesName=javaClass->getInterfacesNames();
        std::list<JavaClass*> interfacesList;
        int interfacesCount= interfacesName.size();
        if(interfacesCount>0){
            JavaClass* interfacesClass;
            for (int i = 0; i < interfacesCount; ++i) {
                interfacesClass=javaClass->getClassLoader()->loadClass(interfacesName[i]);
                interfacesList.push_back(interfacesClass);
            }

            javaClass->setInterfaces(interfacesList);
        }
    }

    //类加载准备过程
    void ClassLoader::prepare(JavaClass *javaClass) {
        calculateInstanceFieldSlotsId(javaClass);
        calculateStaticFieldSlotsId(javaClass);
        allocateAndInitStaticVars(javaClass);


    }

    void ClassLoader::calculateInstanceFieldSlotsId(JavaClass* javaClass) {
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

    void ClassLoader::calculateStaticFieldSlotsId(JavaClass* javaClass) {

        u1 slotId=0;
//        if(javaClass.getSuperClass()!= nullptr){
//            slotId=javaClass.getSuperClass()->getInstanceCount();
//        }

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
        //cout<<javaClass->getStaticCount()<<endl;
    }

    void ClassLoader::allocateAndInitStaticVars(JavaClass* javaClass) {
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





    JavaClass *ClassLoader::loadArrayClass(std::string className) {
        JavaClass* javaClass=new JavaClass(className,this,true);
        this->classMap[className]=javaClass;
        return javaClass;
    }

}