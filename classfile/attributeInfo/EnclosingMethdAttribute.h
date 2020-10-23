//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_ENCLOSINGMETHDATTRIBUTE_H
#define JVM_ENCLOSINGMETHDATTRIBUTE_H

#include "../AttributesInfo.h"

namespace ClassFile{
    class EnclosingMethodAttribute:public AttributesInfo {
    public:
        EnclosingMethodAttribute();

        virtual ~EnclosingMethodAttribute();

        u2 getClassIndex() const;

        void setClassIndex(u2 classIndex);

        u2 getMethodIndex() const;

        void setMethodIndex(u2 methodIndex);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u2 class_index;
        u2 method_index;
    };
}



#endif //JVM_ENCLOSINGMETHDATTRIBUTE_H
