//
// Created by zhukovasky on 2020/8/18.
//

#ifndef JVM_METHODS_H
#define JVM_METHODS_H

#include <string>

namespace ClassFile{
    class Methods {
    public:
        Methods();

        Methods(const std::string &methodName, const std::string &methodAccessFlags,
                const std::string &methodDescriptor);

        const std::string &getMethodName() const;

        void setMethodName(const std::string &methodName);

        const std::string &getMethodAccessFlags() const;

        void setMethodAccessFlags(const std::string &methodAccessFlags);

        const std::string &getMethodDescriptor() const;

        void setMethodDescriptor(const std::string &methodDescriptor);

        virtual ~Methods();


    private:
        std::string methodName;

        std::string methodAccessFlags;

        std::string methodDescriptor;
    };
}



#endif //JVM_METHODS_H
