//
// Created by zhukovasky on 2020/8/4.
//

#ifndef JAVAP_BYTEQUEUE_H
#define JAVAP_BYTEQUEUE_H

#include <iostream>
#include <queue>
#include "../common/Internal.h"
using namespace std;
namespace Lang {
    class ByteQueue {
    public:

        ByteQueue();

        ~ByteQueue();

        u1 popU1();

        u2 popU2();

        u4 popU4();

        long size();

        void pushChar(char byte);

        void pushStringChars(std::string classFiles);

    private:
        std::queue<unsigned char> innerQueue;

    };
}


#endif //JAVAP_BYTEQUEUE_H
