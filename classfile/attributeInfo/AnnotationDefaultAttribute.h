//
// Created by zhukovasky on 2020/8/7.
//

#ifndef JAVAP_ANNOTATIONDEFAULTATTRIBUTE_H
#define JAVAP_ANNOTATIONDEFAULTATTRIBUTE_H

#include "../AttributesInfo.h"
#include "ElementValue.h"

namespace ClassFile{
    class AnnotationDefaultAttribute :public AttributesInfo{
    public:
        AnnotationDefaultAttribute();

        virtual ~AnnotationDefaultAttribute();

        ElementValue *getDefaultValue() const;

        void setDefaultValue(ElementValue *defaultValue);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const string getTags() const override;

        void setTags(std::string tags) override;


    private:
        ElementValue *defaultValue;
    };
}



#endif //JAVAP_ANNOTATIONDEFAULTATTRIBUTE_H
