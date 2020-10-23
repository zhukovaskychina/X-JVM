//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_LOCALVARIABLETABLEATTRIBUTE_H
#define JVM_LOCALVARIABLETABLEATTRIBUTE_H

#include "../AttributesInfo.h"
#include "LocalVariableTable.h"

namespace ClassFile{
    class LocalVariableTableAttribute:public AttributesInfo {
    public:
        LocalVariableTableAttribute();

        virtual ~LocalVariableTableAttribute();

        u2 getLocal_variable_table_length() const;

        void setLocal_variable_table_length(u2 local_variable_table_length);

        LocalVariableTable *getLocalVariableTable() const;

        void setLocalVariableTable(LocalVariableTable *localVariableTable);

        const std::list<LocalVariableTable *> &getLocalVariableTableList() const;

        void setLocalVariableTableList(const std::list<LocalVariableTable *> &localVariableTableList);

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    private:
        u2 local_variable_table_length;
        LocalVariableTable *localVariableTable;
        std::list<LocalVariableTable*> localVariableTableList;
    };
}



#endif //JVM_LOCALVARIABLETABLEATTRIBUTE_H
