//
// Created by zhukovasky on 2020/8/19.
//

#ifndef JVM_JAVAHEAP_H
#define JVM_JAVAHEAP_H

#include "../ClassLoader.h"
#include <set>
namespace Runtime{

        class JavaHeap {



        public:

            Object* createJavaObject(JavaClass* javaClass);

            Object* createJavaArrayObject(JavaClass* javaClass);
        private:
            Runtime::ClassLoader* bootClassLoader;
            std::map<string,ClassFile::ClassFile> constantsPools;
            //新生代
            std::set<Object*> youngList;
            //老生代
            std::set<Object*> eldenList;


        };


}


#endif //JVM_JAVAHEAP_H
