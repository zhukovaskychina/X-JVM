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
    namespace Heap{
        class JString{
        private:

            JString();

            virtual ~JString();
        public:

            static JString* getJString();

            Object* getJString(ClassLoader* classLoader,std::string str);

            Object* getJString(Runtime::MethodArea* methodArea,std::string str);

            std::string getJavaString(Object* object);
        private:
            static JString *jStringInstance;
            std::map<std::string,Object*> stringMap;
        };
    }
}


#endif //JVM_STRINGCONSTANTPOOLS_H
