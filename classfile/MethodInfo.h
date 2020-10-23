//
// Created by zhukovasky on 2020/4/23.
//

#ifndef JVM_METHODINFO_H
#define JVM_METHODINFO_H

#include <list>
#include "../common/Internal.h"
#include "AttributesInfo.h"

namespace ClassFile{
    class MethodInfo {
    public:


        MethodInfo();

        u2 getAccessFlags() const;

        void setAccessFlags(u2 accessFlags);

        u2 getNameIndex() const;

        void setNameIndex(u2 nameIndex);

        u2 getDescriptorIndex() const;

        void setDescriptorIndex(u2 descriptorIndex);

        u2 getAttributesCount() const;

        void setAttributesCount(u2 attributesCount);

//        AttributesInfo **getAttributesInfo() const;
//
//        void setAttributesInfo(AttributesInfo **attributesInfo);

        virtual ~MethodInfo();

        const std::list<AttributesInfo *> &getAttributesInfoList() const;

        void setAttributesInfoList(const std::list<AttributesInfo *> &attributesInfoList);

    public:

    private:
        u2 access_flags;
        u2 name_index;
        u2 descriptor_index;
        u2 attributes_count;
       // AttributesInfo** attributesInfo;
        std::list<AttributesInfo*> attributesInfoList;
    };
};



#endif //JVM_METHODINFO_H
