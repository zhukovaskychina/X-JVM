//
// Created by zhukovasky on 2020/8/7.
//

#ifndef JAVAP_STACKMAPTABLEATTRIBUTES_H
#define JAVAP_STACKMAPTABLEATTRIBUTES_H

#include "../AttributesInfo.h"
#include "StackMapFrame.h"

namespace ClassFile{
    class StackMapTableAttributes:public AttributesInfo {
    public:
        StackMapTableAttributes();

        virtual ~StackMapTableAttributes();

        u2 getNumberOfEntries() const;

        void setNumberOfEntries(u2 numberOfEntries);

        StackMapFrame *getStackMapFrame() const;

        void setStackMapFrame(StackMapFrame *stackMapFrame);

        const std::list<StackMapFrame *> &getStackMapFrameList() const;

        void setStackMapFrameList(const std::list<StackMapFrame *> &stackMapFrameList);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    private:
        u2 numberOfEntries;
        std::list<StackMapFrame*> stackMapFrameList;
        StackMapFrame* stackMapFrame;
    };
}




#endif //JAVAP_STACKMAPTABLEATTRIBUTES_H
