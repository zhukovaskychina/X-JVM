//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_PARAMETERANNOTATIONS_H
#define JAVAP_PARAMETERANNOTATIONS_H

#include "Annotation.h"

namespace ClassFile{
    class ParameterAnnotations {
    public:
        ParameterAnnotations();

        virtual ~ParameterAnnotations();

        u2 getNumAnnotations() const;

        void setNumAnnotations(u2 numAnnotations);

        Annotation *getAnnotations() const;

        void setAnnotations(Annotation *annotations);

    private:
        u2 num_annotations;
        Annotation* annotations;
    };
}



#endif //JAVAP_PARAMETERANNOTATIONS_H
