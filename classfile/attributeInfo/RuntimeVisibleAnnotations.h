//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_RUNTIMEVISIBLEANNOTATIONS_H
#define JAVAP_RUNTIMEVISIBLEANNOTATIONS_H


#include "../AttributesInfo.h"
#include "Annotation.h"

namespace ClassFile{
    class RuntimeVisibleAnnotations:public AttributesInfo{
    public:
        RuntimeVisibleAnnotations();

        virtual ~RuntimeVisibleAnnotations();

        u2 getNumAnnotations() const;

        void setNumAnnotations(u2 numAnnotations);

        Annotation *getAnnotations() const;

        void setAnnotations(Annotation *annotations);

        const list<Annotation *> &getAnnotationsList() const;

        void setAnnotationsList(const list<Annotation *> &annotationsList);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u2 num_annotations;
        Annotation *annotations;
        std::list<Annotation*> annotationsList;
    };

}

#endif //JAVAP_RUNTIMEVISIBLEANNOTATIONS_H
