//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CODE_ATTRIBUTE_H
#define JVM_CODE_ATTRIBUTE_H

#include "../AttributesInfo.h"
#include "Exception_table.h"

namespace ClassFile{
    class Code_attribute:public AttributesInfo {
    public:
        Code_attribute();


        virtual ~Code_attribute();

        u2 getAttributeNameIndex() const override ;

        void setAttributeNameIndex(u2 attributeNameIndex) override ;

        u4 getAttributeLength() const override ;

        void setAttributeLength(u4 attributeLength) override ;

        u2 getMaxStack() const;

        void setMaxStack(u2 maxStack);

        u2 getMaxLocals() const;

        void setMaxLocals(u2 maxLocals);

        u4 getCodeLength() const;

        void setCodeLength(u4 codeLength);

        u1 *getCode() const;

        void setCode(u1 *code);

        u2 getExceptionTableLength() const;

        void setExceptionTableLength(u2 exceptionTableLength);

        Exception_table **getExceptionTable() const;

        void setExceptionTable(Exception_table **exceptionTable);

        u2 getAttributesCount() const;

        void setAttributesCount(u2 attributesCount);

        AttributesInfo *getAttributesInfo() const;

        void setAttributesInfo(AttributesInfo *attributesInfo);

        const std::list<Exception_table *> &getExceptionTableList() const;

        void setExceptionTableList(const std::list<Exception_table *> &exceptionTableList);

        const std::list<AttributesInfo *> &getAttributesInfoList() const;

        void setAttributesInfoList(const std::list<AttributesInfo *> &attributesInfoList);

        const std::string getTags() const override;

        void setTags(std::string tags) override;


    private:
        u2 attribute_name_index;
        u4 attribute_length;
        u2 max_stack;
        u2 max_locals;
        u4 code_length;
        u1 *code;
        u2 exception_table_length;
        Exception_table **exceptionTable;
        std::list<Exception_table*> exceptionTableList;
        u2 attributes_count;
        AttributesInfo *attributesInfo;
        std::list<AttributesInfo*> attributesInfoList;
    };
}



#endif //JVM_CODE_ATTRIBUTE_H
