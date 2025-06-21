//
// Created by zhukovasky on 2020/8/31.
//

#include <cstring>
#include "MethodDescriptor.h"
#include "../../utils/PlatformCompat.h"
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
            if(this->readU1()!='('){

            }
        }

        void MethodDescriptorParser::parseParamTypes() {

        }

        void MethodDescriptorParser::endParams() {
            if(this->readU1()!=')'){

            }
        }

        void MethodDescriptorParser::parseReturnType() {
            if(this->readU1()=='V'){
                this->methodDescriptor->setReturnType("V");
                return;
            }
            this->unreadU1();
            string fieldType=this->parseFieldType();
            if(fieldType!=""){
                this->methodDescriptor->setReturnType(fieldType);
                return;
            }

        }

        void MethodDescriptorParser::finish() {
            if(this->offset!=this->raw.size()){

            }
        }

        MethodDescriptor *MethodDescriptorParser::parse(std::string descriptor) {
            this->raw=descriptor;
            this->offset=0;
            this->charArrays = new char[raw.size()];
            memcpy(this->charArrays, raw.c_str(), raw.length());
            this->methodDescriptor=new MethodDescriptor();
            this->startParams();
            this->parseParamTypes();
            this->endParams();
            this->parseReturnType();
            this->finish();
            return this->methodDescriptor;
        }

        u1 MethodDescriptorParser::readU1() {
            u1 result=getu1(this->charArrays[this->offset]);
            this->offset++;
            return result;
        }

        void MethodDescriptorParser::unreadU1() {
            this->offset--;
        }

        std::string MethodDescriptorParser::parseObjectType() {
            std::string unread=this->readRest();
            int pos=unread.find(";",0);
            if(pos==-1){
                //报错
                cerr<<"此处报错"<<endl;
                exit(1);
            }
            int start=this->offset-1;
            int end=this->offset+pos+1;

            return std::string(this->readSpecifiedCharsInPositions(start,end));
        }

        std::string MethodDescriptorParser::parseArrayType() {
            int start=this->offset-1;
            this->parseFieldType();
            int end=this->offset;
            return std::string(this->readSpecifiedCharsInPositions(start,end));
        }

        std::string MethodDescriptorParser::parseFieldType() {
            u1 type=this->readU1();
            switch(type){
                case 'B':{
                    return "B";
                }
                case 'C':{
                    return "C";
                }
                case 'D':{
                    return "D";
                }
                case 'F':{
                    return "F";
                }
                case 'I':{
                    return "I";
                }
                case 'J':{
                    return "J";
                }
                case 'S':{
                    return "S";
                }
                case 'Z':{
                    return "Z";
                }
                case 'L':{
                    return this->parseObjectType();
                }
                case '[':{
                    return this->parseArrayType();
                }
                default:{
                    "";
                }
            }
        }

        char* MethodDescriptorParser::readRest() {
            int size = this->raw.size();
            char* res = new char[size - this->offset + 1];
            for (int i = this->offset; i < size; ++i) {
                res[i - this->offset] = this->charArrays[i];
            }
            res[size - this->offset] = '\0';
            return res;
        }

        std::string MethodDescriptorParser::readSpecifiedCharsInPositions(int start, int end) {
            std::string result;
            for (int i = start; i < end; ++i) {
                result += this->charArrays[i];
            }
            return result;
        }

        std::string MethodDescriptor::getParameterTypes() {
            // 返回参数类型的字符串表示
            std::string result = "(";
            for (const auto& param : parameterType) {
                result += param;
            }
            result += ")";
            return result;
        }
    }
}