//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_ELEMENTVALUE_H
#define JVM_ELEMENTVALUE_H


#include "../../common/Internal.h"
#include "../../lang/ByteQueue.h"

namespace ClassFile{
    class ElementValue {
    public:
        ElementValue(Lang::ByteQueue *byteQueue);

        ElementValue();

        virtual ~ElementValue();

        u1 getTags() const;

        void setTags(u1 tags);

    public:


    private:
        u1 tags;
    };
}


#endif //JVM_ELEMENTVALUE_H
