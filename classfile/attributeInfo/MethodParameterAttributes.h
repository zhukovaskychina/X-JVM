//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_METHODPARAMETERATTRIBUTES_H
#define JAVAP_METHODPARAMETERATTRIBUTES_H

#include "../AttributesInfo.h"
#include "Parameters.h"

namespace ClassFile{
    class MethodParameterAttributes: public AttributesInfo {
    public:
        MethodParameterAttributes();

        virtual ~MethodParameterAttributes();

        u1 getParameterCounts() const;

        void setParameterCounts(u1 parameterCounts);

        Parameters *getParameters() const;

        void setParameters(Parameters *parameters);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    private:
        u1 parameter_counts;
        Parameters* parameters;
    };

}


#endif //JAVAP_METHODPARAMETERATTRIBUTES_H
