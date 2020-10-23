//
// Created by zhukovasky on 2020/8/17.
//

#include "JavaRuntime.h"

namespace Runtime{
    void JavaRuntimeEnv::initRuntime() {

    }

    void JavaRuntimeEnv::initRuntime(std::string xjre, std::string classPathName, std::string mainClassName,
                                     std::string cpOtions) {

        this->classPath=new ClassPath();
        this->classPath->parseBootAndExtClassPath(xjre);
        this->systemClassLoader=new ClassLoader(this->classPath);
        this->classPath->parseUserClassPath(classPathName);
        this->userClassLoader=new ClassLoader(this->classPath);

        //初始化堆
        this->javaHeap=new JavaHeap();
        this->jStringConstantPools=Runtime::Heap::JString::getJString();
        //方法区
        this->methodArea=new MethodArea();
    }



}
