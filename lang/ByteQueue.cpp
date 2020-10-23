//
// Created by zhukovasky on 2020/8/4.
//

#include "ByteQueue.h"
#include <string.h>
Lang::ByteQueue::ByteQueue() {}

Lang::ByteQueue::~ByteQueue() {

}

void Lang::ByteQueue::pushChar(char byte) {}

void Lang::ByteQueue::pushStringChars(std::string classFiles) {
    char *classContent = new char[classFiles.length()];
    memcpy(classContent, classFiles.c_str(), classFiles.length());
    long size=classFiles.size();
    for (int i = 0; i < size; ++i) {
        this->innerQueue.push(classContent[i]);
    }

}

u1 Lang::ByteQueue::popU1() {
    if(this->size()==0){
        throw new exception();
    }
    char firstChar[1];
    firstChar[0]=this->innerQueue.front();
    this->innerQueue.pop();
    return  getu1(firstChar);
}

u2 Lang::ByteQueue::popU2() {
    char chars[2];
    chars[0]=this->innerQueue.front();
    this->innerQueue.pop();
    chars[1]=this->innerQueue.front();
    this->innerQueue.pop();
    return getu2(chars);
}

u4 Lang::ByteQueue::popU4() {
    char chars[4];
    for (int i = 0; i < 4; ++i) {
        char temp=  this->innerQueue.front();
        this->innerQueue.pop();
        chars[i]=temp;
    }
    return getu4(chars);
}
long Lang::ByteQueue::size() {
    return this->innerQueue.size();
}
