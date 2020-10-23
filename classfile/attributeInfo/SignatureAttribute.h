//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_SIGNATUREATTRIBUTE_H
#define JVM_SIGNATUREATTRIBUTE_H

#include "../AttributesInfo.h"

namespace ClassFile{

    class SignatureAttribute:public AttributesInfo {
    public:
        SignatureAttribute();

        virtual ~SignatureAttribute();

        u2 getSignatureIndex() const;

        void setSignatureIndex(u2 signatureIndex);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u2 signature_index;
    };
}



#endif //JVM_SIGNATUREATTRIBUTE_H
