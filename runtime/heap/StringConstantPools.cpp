//
// Created by zhukovasky on 2020/9/2.
//

#include "StringConstantPools.h"
#include "../../utils/StringUtils.h"

namespace Runtime{
    namespace Heap{

        JString* JString::jStringInstance;
        Object* JString::getJString(ClassLoader *classLoader, std::string str) {
            Object *obj=stringMap[str];
            if(obj!= nullptr){
                return obj;
            }
          //  u16string u16Str=Utils::StringUtils::convertStringToWS(str);
          //  const char16_t * chars=u16Str.c_str();
            Object *object=new Object();
            JavaClass* javaClass=classLoader->loadClass("java/lang/String");
            object->setJavaClass(javaClass);
            char* result=new char[str.length()+1];
            strcpy(result,str.c_str());
            object->setData((void*)(result));
            object->setObjectType("char");
            stringMap[str]=object;
            return object;
        }

        std::string JString::getJavaString(Object *object) {
            char * charData=(char *)object->getData();
      //      string result=Utils::StringUtils::convertWStringToString(charData);
            return std::__cxx11::string(charData);
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

        Object *JString::getJString(Runtime::MethodArea *methodArea, std::string str) {
            Object *obj=stringMap[str];
            if(obj!= nullptr){
                return obj;
            }
            //  u16string u16Str=Utils::StringUtils::convertStringToWS(str);
            //  const char16_t * chars=u16Str.c_str();
            Object *object=new Object();
            JavaClass* javaClass=methodArea->loadClass("java/lang/String");
            object->setJavaClass(javaClass);
            char* result=new char[str.length()+1];
            strcpy(result,str.c_str());
            object->setData((void*)(result));
            object->setObjectType("char");
            stringMap[str]=object;
            return object;
        }
    }
}