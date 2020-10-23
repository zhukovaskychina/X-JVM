//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_CLASSLOADER_H
#define JAVAP_CLASSLOADER_H

#include <string>
#include <map>
#include <list>
#include "heap/Entry.h"
#include "heap/ClassMember.h"


namespace Runtime{

    class JavaClass;
    class FieldInfo;
    class ClassPath{
    public:


        ClassPath();

        Heap::Entry* readClass(std::string className);

        ClassPath* parse(std::string jreOptions,std::string cpOptions);

        ClassPath* parseBootAndExtClassPath(std::string jreOptions);

        ClassPath* parseUserClassPath(std::string cpOption);

        virtual ~ClassPath();

        Heap::Entry **getBootClassPath() const;

        void setBootClassPath(Heap::Entry **bootClassPath);

        Heap::Entry **getExtClassPath() const;

        void setExtClassPath(Heap::Entry **extClassPath);

        Heap::Entry *getUserClassPath() const;

        const std::list<Heap::Entry *> &getBootClassPathEntryList() const;

        const std::list<Heap::Entry *> &getExtClassPathEntryList() const;

        const std::list<Heap::Entry *> &getUserClassPathEntryList() const;



    private:

        std::list<Heap::Entry*> getWildCardEntry(std::string path);

        std::string getJreHome();

        std::list<Heap::Entry*> bootClassPathEntryList;

        std::list<Heap::Entry*> extClassPathEntryList;

        std::list<Heap::Entry*> userClassPathEntryList;

    };

    class ClassLoader {
    public:
        ClassLoader(ClassPath *classPath);
        ClassLoader* getBootstrapClassLoader(std::string jreOptions);
        JavaClass* loadClass(std::string className);
        JavaClass* loadArrayClass(std::string className);
        JavaClass* loadNonArrayClass(std::string className);
        Heap::Entry* readClass(std::string className);
        JavaClass* defineClass(Lang::ByteQueue* data);
        JavaClass* parseClass(u1* data);
        void resolveSuperClass(Runtime::JavaClass *javaClass);
        void resolveInterfaces(JavaClass *javaClass);
        void link(JavaClass *javaClass);
        void prepare(JavaClass *javaClass);
    private:

        //初始化bootstrap classloader
        void initBootStrapClassLoader(std::string jreOptions);
        ClassPath *classPath;
        std::map<std::string,JavaClass*> classMap;

        void calculateInstanceFieldSlotsId(JavaClass *javaClass);

        void calculateStaticFieldSlotsId(JavaClass *javaClass);

        void allocateAndInitStaticVars(JavaClass *javaClass);


    };
}


#endif //JAVAP_CLASSLOADER_H
