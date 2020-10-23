//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_LOCALVRIABLETYPETABLEATTRIBUTE_H
#define JVM_LOCALVRIABLETYPETABLEATTRIBUTE_H

#include "../AttributesInfo.h"
#include "LocalVariableTypeTable.h"

namespace ClassFile{
    class LocalVariableTypeTableAttribute:public AttributesInfo{
    public:


        LocalVariableTypeTableAttribute();

        u2 getLocalVariableTypeTableLength() const;

        void setLocalVariableTypeTableLength(u2 localVariableTypeTableLength);

        LocalVariableTypeTable *getLocalVriableTypeTable() const;

        void setLocalVriableTypeTable(LocalVariableTypeTable *localVriableTypeTable);

        virtual ~LocalVariableTypeTableAttribute();

    private:
        u2 local_variable_type_table_length;
        LocalVariableTypeTable *localVriableTypeTable;
    };
}




#endif //JVM_LOCALVRIABLETYPETABLEATTRIBUTE_H
