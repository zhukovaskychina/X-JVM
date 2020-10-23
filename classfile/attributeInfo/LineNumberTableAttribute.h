//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_LINENUMBERTABLEATTRIBUTE_H
#define JVM_LINENUMBERTABLEATTRIBUTE_H

#include "../AttributesInfo.h"
#include "LineNumberTable.h"

namespace ClassFile{

    class LineNumberTableAttribute:public AttributesInfo{
    public:
        LineNumberTableAttribute();

        virtual ~LineNumberTableAttribute();

        u2 getLineNumberTableLength() const;

        void setLineNumberTableLength(u2 lineNumberTableLength);

        LineNumberTable *getLineNumberTable() const;

        void setLineNumberTable(LineNumberTable *lineNumberTable);

        const std::list<LineNumberTable *> getLineNumberTableList() const;

        void setLineNumberTableList(const std::list<LineNumberTable *> &lineNumberTableList);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;


    private:
        u2 line_number_table_length;
        std::list<LineNumberTable*> lineNumberTableList;
        LineNumberTable *lineNumberTable;
    };
}



#endif //JVM_LINENUMBERTABLEATTRIBUTE_H
