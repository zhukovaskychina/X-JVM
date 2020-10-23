//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_RUNTIMEVISIBLEPARAMETERANNOTATIONATTRIBUTES_H
#define JAVAP_RUNTIMEVISIBLEPARAMETERANNOTATIONATTRIBUTES_H


#include "../AttributesInfo.h"
#include "ParameterAnnotations.h"

namespace ClassFile{
    class RuntimeVisibleParameterAnnotationAttributes :public AttributesInfo{
    public:
        RuntimeVisibleParameterAnnotationAttributes();

        virtual ~RuntimeVisibleParameterAnnotationAttributes();

        u1 getNumParameters() const;

        void setNumParameters(u1 numParameters);

        ParameterAnnotations *getParameterAnnotations() const;

        void setParameterAnnotations(ParameterAnnotations *parameterAnnotations);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const string getTags() const override;

        void setTags(std::string tags) override;

    private:
        u1 numParameters;
        ParameterAnnotations* parameterAnnotations;
    };
}




#endif //JAVAP_RUNTIMEVISIBLEPARAMETERANNOTATIONATTRIBUTES_H
