//
// Created by zhukovasky on 2020/8/17.
//

#ifndef JAVAP_JAVARUNTIME_H
#define JAVAP_JAVARUNTIME_H

#include <map>
#include "iostream"
#include "heap/JavaHeap.h"
#include "MethodArea.h"
#include "heap/StringConstantPools.h"

namespace Runtime{
    class JavaRuntimeEnv {

    public:
        JavaRuntimeEnv();
        ~JavaRuntimeEnv();

        void initRuntime();

        void initRuntime(std::string xjre,std::string classPathName,std::string mainClassName,std::string cpOtions);



        long getHeapMemorySize();

        void exit();
    private:
        //java堆
        Runtime::Heap::JString *jStringConstantPools;
        Runtime::JavaHeap *javaHeap;
        Runtime::ClassPath *classPath;
        Runtime::ClassLoader *systemClassLoader;
        Runtime::ClassLoader *userClassLoader;
        Runtime::MethodArea *methodArea;
    };
}



#endif //JAVAP_JAVARUNTIME_H
