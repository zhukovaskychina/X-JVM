//
// Created by zhukovasky on 2020/8/11.
//

#include "StringConstantPools.h"
#include "JavaHeap.h"
#include "../../utils/PlatformCompat.h"

namespace Runtime{
    namespace Heap{
        JString* JString::jStringInstance;
        Object* JString::getJString(ClassLoader *classLoader, std::string str, Runtime::JavaHeap *javaHeap) {
            Object *obj=stringMap[str];
            if(obj!= nullptr){
                return obj;
            }
            JavaClass* javaClass=classLoader->loadClass("java/lang/String");
            Object *object = nullptr;
            if (javaHeap != nullptr) {
                object = javaHeap->createJavaObject(javaClass);
            } else {
                object = new Object();
                object->setJavaClass(javaClass);
            }
            char* result=new char[str.length()+1];
            strcpy(result,str.c_str());
            object->setData(static_cast<void*>(result));
            object->setObjectType("char");
            stringMap[str]=object;
            return object;
        }

        std::string JString::getJavaString(Object *object) {
            char * charData=(char *)object->getData();
      //      string result=Utils::StringUtils::convertWStringToString(charData);
            return std::string(charData);
        }

        JString *JString::getJString() {
            if(jStringInstance== nullptr){
                jStringInstance=new JString();
                return jStringInstance;
            }
            return jStringInstance;
        }

        JString::JString() {}

        JString::~JString() {

        }

        Object *JString::getJString(Runtime::MethodArea *methodArea, std::string str, Runtime::JavaHeap *javaHeap) {
            Object *obj=stringMap[str];
            if(obj!= nullptr){
                return obj;
            }
            JavaClass* javaClass=methodArea->loadClass("java/lang/String");
            Object *object = nullptr;
            if (javaHeap != nullptr) {
                object = javaHeap->createJavaObject(javaClass);
            } else {
                object = new Object();
                object->setJavaClass(javaClass);
            }
            char* result=new char[str.length()+1];
            strcpy(result,str.c_str());
            object->setData(static_cast<void*>(result));
            object->setObjectType("char");
            stringMap[str]=object;
            return object;
        }
    }
}