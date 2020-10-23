//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_LOCALVARIABLETABLE_H
#define JVM_LOCALVARIABLETABLE_H


#include "../../common/Internal.h"

namespace ClassFile{
    class LocalVariableTable {
    public:
        LocalVariableTable();

        virtual ~LocalVariableTable();

        u2 getStart_pc() const;

        void setStart_pc(u2 start_pc);

        u2 getLength() const;

        void setLength(u2 length);

        u2 getName_index() const;

        void setName_index(u2 name_index);

        u2 getDescriptor_index() const;

        void setDescriptor_index(u2 descriptor_index);

        u2 getIndex() const;

        void setIndex(u2 index);

    public:

    private:
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 descriptor_index;
        u2 index;
    };

}



#endif //JVM_LOCALVARIABLETABLE_H
