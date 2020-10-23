//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_BOOTSTRAPMETHODATTRIBUTE_H
#define JAVAP_BOOTSTRAPMETHODATTRIBUTE_H

#include "../AttributesInfo.h"
#include "BootstrapMethod.h"

namespace ClassFile{
    class BootstrapMethodAttribute : public AttributesInfo {
    public:
        BootstrapMethodAttribute();

        virtual ~BootstrapMethodAttribute();

        u2 getNumBootstrapMethods() const;

        void setNumBootstrapMethods(u2 numBootstrapMethods);

        BootstrapMethod* getBootstrapMethod() const;

        void setBootstrapMethod(BootstrapMethod* bootstrapMethod);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

        const std::list<BootstrapMethod *> getBootstrapMethodList() const;

        void setBootstrapMethodList(const std::list<BootstrapMethod *> &bootstrapMethodList);


    private:
        u2 numBootstrapMethods;
        BootstrapMethod* bootstrapMethod;
        std::list<BootstrapMethod*> bootstrapMethodList;
    };
}



#endif //JAVAP_BOOTSTRAPMETHODATTRIBUTE_H
