//
// Created by zhukovasky on 2020/9/2.
//

#ifndef JVM_STRINGCONSTANTPOOLS_H
#define JVM_STRINGCONSTANTPOOLS_H

#include <map>
#include "../Object.h"
#include "../ClassLoader.h"
#include "../MethodArea.h"
namespace Runtime{
    class JavaHeap;
    namespace Heap{
        class JString{
        private:

            JString();

            virtual ~JString();
        public:

            static JString* getJString();

            /**
             * 字符串驻留。若 javaHeap 非空，新字符串对象经堆分配并纳入 GC；否则沿用堆外 new（类加载等无堆上下文路径）。
             */
            Object* getJString(ClassLoader* classLoader, std::string str, Runtime::JavaHeap* javaHeap = nullptr);

            Object* getJString(Runtime::MethodArea* methodArea, std::string str, Runtime::JavaHeap* javaHeap = nullptr);

            std::string getJavaString(Object* object);

            /** GC 根扫描：对每个驻留的 java.lang.String 实例执行 fn(Object*)。 */
            template<typename Fn>
            void forEachInternedObject(Fn&& fn) const {
                for (const auto& kv : stringMap) {
                    if (kv.second) {
                        fn(kv.second);
                    }
                }
            }

        private:
            static JString *jStringInstance;
            std::map<std::string,Object*> stringMap;
        };
    }
}


#endif //JVM_STRINGCONSTANTPOOLS_H
