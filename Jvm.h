//
// Created by zhukovasky on 2020/8/17.
//

#ifndef JVM_JVM_H
#define JVM_JVM_H


#include "runtime/ThreadPool.h"

class Jvm {
public:
     static Runtime::ExecutorThreadPool executor;
};


#endif //JVM_JVM_H
