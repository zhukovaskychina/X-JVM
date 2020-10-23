//
// Created by zhukovasky on 2020/8/31.
//

#ifndef JVM_METHODDESCRIPTOR_H
#define JVM_METHODDESCRIPTOR_H

#include <list>
#include <iostream>
#include "../../common/Internal.h"

using namespace std;
namespace Runtime{
    namespace Heap{
        class MethodDescriptor {
        public:
            MethodDescriptor();

            const list<std::string> &getParameterType() const;

            void setParameterType(const list<std::string> &parameterType);

            const string &getReturnType() const;

            void setReturnType(const string &returnType);

            virtual ~MethodDescriptor();


        private:
            std::list<std::string> parameterType;

            std::string returnType;
        };
        class MethodDescriptorParser{
        public:

            MethodDescriptor* parse(std::string descriptor);


        private:
            std::string raw;

            char* charArrays;

            int offset;
            MethodDescriptor* methodDescriptor;

            void startParams();

            void parseParamTypes();

            void endParams();

            void parseReturnType();

            string parseFieldType();

            void finish();

            u1 readU1();

            char* readRest();

            void unreadU1();

            std::string parseObjectType();

            std::string parseArrayType();

            std::string readSpecifiedCharsInPositions(int start,int end);

        };
    }
}


#endif //JVM_METHODDESCRIPTOR_H
