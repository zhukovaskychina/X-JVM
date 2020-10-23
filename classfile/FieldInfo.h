//
// Created by zhukovasky on 2020/4/23.
//

#ifndef JVM_FIELDINFO_H
#define JVM_FIELDINFO_H


#include "../common/Internal.h"
#include "AttributesInfo.h"

namespace ClassFile{

    class FieldInfo{
    public:
        FieldInfo();

        virtual ~FieldInfo();

        u2 getAccessFlags() const;

        void setAccessFlags(u2 accessFlags);

        u2 getNameIndex() const;

        void setNameIndex(u2 nameIndex);

        u2 getDescriptorIndex() const;

        void setDescriptorIndex(u2 descriptorIndex);

        u2 getAttributesCount() const;

        void setAttributesCount(u2 attributesCount);

        const std::list<AttributesInfo *> &getAttributesInfoList() const;

        void setAttributesInfoList(const std::list<AttributesInfo *> &attributesInfoList);


    private:
        u2 access_flags;
        u2 name_index;
        u2 descriptor_index;
        u2 attributes_count;
        std::list<AttributesInfo*> attributesInfoList;
        AttributesInfo* attributesInfo;
    };
};

#endif //JVM_FIELDINFO_H
