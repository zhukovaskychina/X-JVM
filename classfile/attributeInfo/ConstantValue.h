//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANTVALUE_H
#define JVM_CONSTANTVALUE_H

#include "../AttributesInfo.h"

namespace ClassFile {
    class ConstantValue : public AttributesInfo {

    public:
        ConstantValue();

        virtual ~ConstantValue();

        u2 getConstantvalueIndex() const;

        void setConstantvalueIndex(u2 constantvalueIndex);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;


    private:
        u2 constantvalue_index;
    };

}


#endif //JVM_CONSTANTVALUE_H
