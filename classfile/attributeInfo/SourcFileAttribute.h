//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_SOURCFILEATTRIBUTE_H
#define JVM_SOURCFILEATTRIBUTE_H


#include "../../common/Internal.h"
#include "../AttributesInfo.h"

namespace ClassFile{
    class SourceFileAttribute:public AttributesInfo{
    public:
        SourceFileAttribute();

        virtual ~SourceFileAttribute();

        u2 getSourcefile_index() const;

        void setSourcefile_index(u2 sourcefile_index);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u2 sourcefile_index;
    };
}

#endif //JVM_SOURCFILEATTRIBUTE_H
