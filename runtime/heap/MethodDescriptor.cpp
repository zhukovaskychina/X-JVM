//
// Created by zhukovasky on 2020/8/31.
//

#include "MethodDescriptor.h"

namespace Runtime{
    namespace Heap{

        const list<std::string> &MethodDescriptor::getParameterType() const {
            return parameterType;
        }

        void MethodDescriptor::setParameterType(const list<std::string> &parameterType) {
            MethodDescriptor::parameterType = parameterType;
        }

        const string &MethodDescriptor::getReturnType() const {
            return returnType;
        }

        void MethodDescriptor::setReturnType(const string &returnType) {
            MethodDescriptor::returnType = returnType;
        }

        MethodDescriptor::MethodDescriptor() {}

        MethodDescriptor::~MethodDescriptor() {
            this->returnType.clear();
            this->parameterType.clear();
        }

        void MethodDescriptorParser::startParams() {
            if (offset >= static_cast<int>(raw.size())) {
                ok_ = false;
                return;
            }
            if (static_cast<unsigned char>(charArrays[offset]) != '(') {
                ok_ = false;
                return;
            }
            offset++;
        }

        void MethodDescriptorParser::parseParamTypes() {
            std::list<std::string> params;
            while (offset < static_cast<int>(raw.size())) {
                u1 c = readU1();
                if (c == ')') {
                    unreadU1();
                    break;
                }
                unreadU1();
                std::string ft = parseFieldType();
                if (ft.empty()) {
                    ok_ = false;
                    return;
                }
                params.push_back(ft);
            }
            if (ok_) {
                methodDescriptor->setParameterType(params);
            }
        }

        void MethodDescriptorParser::endParams() {
            if (offset >= static_cast<int>(raw.size()) || readU1() != ')') {
                ok_ = false;
            }
        }

        void MethodDescriptorParser::parseReturnType() {
            if (offset >= static_cast<int>(raw.size())) {
                ok_ = false;
                return;
            }
            if (readU1() == 'V') {
                this->methodDescriptor->setReturnType("V");
                return;
            }
            this->unreadU1();
            std::string fieldType = this->parseFieldType();
            if (!fieldType.empty()) {
                this->methodDescriptor->setReturnType(fieldType);
                return;
            }
            ok_ = false;
        }

        void MethodDescriptorParser::finish() {
            if (offset != static_cast<int>(raw.size())) {
                ok_ = false;
            }
        }

        MethodDescriptor *MethodDescriptorParser::parse(std::string descriptor) {
            ok_ = true;
            raw = std::move(descriptor);
            storage_.assign(raw.begin(), raw.end());
            charArrays = storage_.empty() ? nullptr : storage_.data();
            offset = 0;
            methodDescriptor = new MethodDescriptor();
            startParams();
            if (ok_) {
                parseParamTypes();
            }
            if (ok_) {
                endParams();
            }
            if (ok_) {
                parseReturnType();
            }
            if (ok_) {
                finish();
            }
            if (!ok_) {
                delete methodDescriptor;
                methodDescriptor = nullptr;
                return nullptr;
            }
            return methodDescriptor;
        }

        u1 MethodDescriptorParser::readU1() {
            if (offset >= static_cast<int>(raw.size()) || !charArrays) {
                ok_ = false;
                return 0;
            }
            u1 result = static_cast<u1>(static_cast<unsigned char>(charArrays[offset]));
            offset++;
            return result;
        }

        void MethodDescriptorParser::unreadU1() {
            if (offset > 0) {
                offset--;
            }
        }

        std::string MethodDescriptorParser::parseObjectType() {
            const int start = this->offset - 1;
            const std::size_t pos = raw.find(';', static_cast<std::size_t>(this->offset));
            if (pos == std::string::npos) {
                ok_ = false;
                return "";
            }
            const int end = static_cast<int>(pos) + 1;
            this->offset = end;
            return this->readSpecifiedCharsInPositions(start, end);
        }

        std::string MethodDescriptorParser::parseArrayType() {
            int start = this->offset - 1;
            this->parseFieldType();
            int end = this->offset;
            return std::string(this->readSpecifiedCharsInPositions(start, end));
        }

        std::string MethodDescriptorParser::parseFieldType() {
            u1 type = this->readU1();
            switch (type) {
                case 'B':
                    return "B";
                case 'C':
                    return "C";
                case 'D':
                    return "D";
                case 'F':
                    return "F";
                case 'I':
                    return "I";
                case 'J':
                    return "J";
                case 'S':
                    return "S";
                case 'Z':
                    return "Z";
                case 'L':
                    return this->parseObjectType();
                case '[':
                    return this->parseArrayType();
                default:
                    return "";
            }
        }

        std::string MethodDescriptorParser::readSpecifiedCharsInPositions(int start, int end) {
            std::string result;
            if (!charArrays || start < 0 || end > static_cast<int>(raw.size()) || start > end) {
                ok_ = false;
                return result;
            }
            for (int i = start; i < end; ++i) {
                result += charArrays[i];
            }
            return result;
        }

        std::string MethodDescriptor::getParameterTypes() {
            std::string result = "(";
            for (const auto& param : parameterType) {
                result += param;
            }
            result += ")";
            return result;
        }
    }
}
