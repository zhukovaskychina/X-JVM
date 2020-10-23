//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_LOCALVARIABLETYPETABLE_H
#define JVM_LOCALVARIABLETYPETABLE_H


#include "../../common/Internal.h"

namespace ClassFile{

    class LocalVariableTypeTable {
    public:
        LocalVariableTypeTable();

        virtual ~LocalVariableTypeTable();

        u2 getStartPc() const;

        void setStartPc(u2 startPc);

        u2 getLength() const;

        void setLength(u2 length);

        u2 getNameIndex() const;

        void setNameIndex(u2 nameIndex);

        u2 getSignatureIndex() const;

        void setSignatureIndex(u2 signatureIndex);

        u2 getIndex() const;

        void setIndex(u2 index);

    public:

    private:
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 signature_index;
        u2 index;
    };

}


#endif //JVM_LOCALVARIABLETYPETABLE_H
