//
// Created by zhukovasky on 2020/8/18.
//

#include "Methods.h"

namespace ClassFile{
    const std::string &Methods::getMethodName() const {
        return methodName;
    }

    void Methods::setMethodName(const std::string &methodName) {
        Methods::methodName = methodName;
    }

    const std::string &Methods::getMethodAccessFlags() const {
        return methodAccessFlags;
    }

    void Methods::setMethodAccessFlags(const std::string &methodAccessFlags) {
        Methods::methodAccessFlags = methodAccessFlags;
    }

    const std::string &Methods::getMethodDescriptor() const {
        return methodDescriptor;
    }

    void Methods::setMethodDescriptor(const std::string &methodDescriptor) {
        Methods::methodDescriptor = methodDescriptor;
    }

    Methods::Methods(const std::string &methodName, const std::string &methodAccessFlags,
                     const std::string &methodDescriptor) : methodName(methodName),
                                                            methodAccessFlags(methodAccessFlags),
                                                            methodDescriptor(methodDescriptor) {
        this->methodName=methodName;
        this->methodAccessFlags=methodAccessFlags;
        this->methodDescriptor=methodDescriptor;
    }

    Methods::Methods() {}

    Methods::~Methods() {

    }
}