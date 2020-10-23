//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_INNERCLASSES_H
#define JVM_INNERCLASSES_H

#include "../AttributesInfo.h"
#include "ClassessInner.h"

namespace ClassFile{
    class InnerClasses:public AttributesInfo{
    public:
        InnerClasses();

        virtual ~InnerClasses();

        u2 getNumberOfClasses() const;

        void setNumberOfClasses(u2 numberOfClasses);

        ClassessInner *getClassess() const;

        void setClassess(ClassessInner *classess);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u2 number_of_classes;
        ClassessInner *classess;
    };
}



#endif //JVM_INNERCLASSES_H
