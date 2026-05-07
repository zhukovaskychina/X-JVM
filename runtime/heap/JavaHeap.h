//
// Created by zhukovasky on 2020/8/19.
//

#ifndef JVM_JAVAHEAP_H
#define JVM_JAVAHEAP_H

#include "../ClassLoader.h"
#include <cstddef>
#include <set>
#include <string>
namespace Runtime{

        class Object;

        class JavaHeap {



        public:

            Object* createJavaObject(JavaClass* javaClass);

            Object* createJavaArrayObject(JavaClass* javaClass);

            /** Clear mark bit on every heap-allocated object. */
            void clearGcMarksOnAllObjects();

            /** Remove unmarked objects from tracking and delete them; clear marks on survivors. */
            void sweepUnmarkedObjects();

            std::size_t totalTrackedObjects() const;

            /** 自上次 GC 完成以来的分配次数（每次 createJavaObject / createJavaArrayObject +1）。 */
            std::size_t allocationsSinceLastGc() const;

            /** 达到该次数后 shouldCollect() 为 true；默认较大，避免无堆路径误触发。 */
            std::size_t gcAllocationThreshold() const;
            void setGcAllocationThreshold(std::size_t n);

            bool shouldCollect() const;

            /** 由 GC 在 sweep 完成后调用，清零分配计数。 */
            void onGcCompleted();

        private:
            void recordAllocation();

            Runtime::ClassLoader* bootClassLoader;
            std::map<std::string,ClassFile::ClassFile> constantsPools;
            //新生代
            std::set<Object*> youngList;
            //老生代
            std::set<Object*> eldenList;

            std::size_t allocationsSinceLastGc_{0};
            std::size_t gcAllocationThreshold_{256};
        };


}


#endif //JVM_JAVAHEAP_H
