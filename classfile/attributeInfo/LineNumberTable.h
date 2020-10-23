//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_LINENUMBERTABLE_H
#define JVM_LINENUMBERTABLE_H

#include "../../common/Internal.h"

namespace ClassFile{
    class LineNumberTable {
    public:
        LineNumberTable();

        virtual ~LineNumberTable();

        u2 getStart_pc() const;

        void setStart_pc(u2 start_pc);

        u2 getLine_number() const;

        void setLine_number(u2 line_number);

    private:
        u2 start_pc;
        u2 line_number;
    };

}



#endif //JVM_LINENUMBERTABLE_H
