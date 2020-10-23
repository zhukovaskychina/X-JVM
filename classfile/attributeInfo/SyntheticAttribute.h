//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_SYNTHETICATTRIBUTE_H
#define JVM_SYNTHETICATTRIBUTE_H


#include "../AttributesInfo.h"

namespace ClassFile{
    class SyntheticAttribute:public AttributesInfo {
    public:
        SyntheticAttribute();

        virtual ~SyntheticAttribute();

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    };

}


#endif //JVM_SYNTHETICATTRIBUTE_H
