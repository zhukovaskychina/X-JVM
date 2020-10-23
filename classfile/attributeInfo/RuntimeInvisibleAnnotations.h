//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_RUNTIMEINVISIBLEANNOTATIONS_H
#define JAVAP_RUNTIMEINVISIBLEANNOTATIONS_H

#include "../AttributesInfo.h"
#include "Annotation.h"

namespace ClassFile{
    class RuntimeInvisibleAnnotations :public AttributesInfo{
    public:
        RuntimeInvisibleAnnotations();

        u2 getNumAnnotations() const;

        void setNumAnnotations(u2 numAnnotations);

        Annotation *getAnnotations() const;

        void setAnnotations(Annotation *annotations);

        virtual ~RuntimeInvisibleAnnotations();

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const string getTags() const override;

        void setTags(std::string tags) override;

    private:
        u2 num_annotations;
        Annotation *annotations;
    };
}



#endif //JAVAP_RUNTIMEINVISIBLEANNOTATIONS_H
