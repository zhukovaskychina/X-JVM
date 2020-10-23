//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_SOURCEDEBUGEXTENSIONATTRIBUTE_H
#define JVM_SOURCEDEBUGEXTENSIONATTRIBUTE_H


#include "../AttributesInfo.h"

namespace ClassFile{
    class SourceDebugExtensionAttribute:public AttributesInfo {
    public:
        SourceDebugExtensionAttribute();

        virtual ~SourceDebugExtensionAttribute();

        u1 *getDebugExtension() const;

        void setDebugExtension(u1 *debugExtension);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u1 *debug_extension;
    };

}



#endif //JVM_SOURCEDEBUGEXTENSIONATTRIBUTE_H
