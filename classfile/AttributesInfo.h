//
// Created by zhukovasky on 2020/4/23.
//

#ifndef JVM_ATTRIBUTESINFO_H
#define JVM_ATTRIBUTESINFO_H

#include "../common/Internal.h"
#include "ConstantPools.h"
#include "ConstantPoolsList.h"

namespace ClassFile{
    class AttributesInfo {
    public:
        AttributesInfo();
        virtual ~AttributesInfo();


        virtual void setAttributeNameIndex(u2 attributeNameIndex)=0;

        virtual void setAttributeLength(u4 attributeLength)=0;


        virtual u2 getAttributeNameIndex() const=0;

        virtual u4 getAttributeLength() const=0;

        virtual const std::string getTags() const=0;

        virtual void setTags(std::string tags)=0;

    private:
        u2 attribute_name_index;
        u4 attribute_length;
        std::string tags;

        ConstantPoolsList* constantPoolsList;
    };
};



#endif //JVM_ATTRIBUTESINFO_H
