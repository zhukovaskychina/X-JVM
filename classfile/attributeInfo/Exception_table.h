//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_EXCEPTION_TABLE_H
#define JVM_EXCEPTION_TABLE_H


#include "../../common/Internal.h"

class Exception_table {
public:
    Exception_table();

    Exception_table(u2 startPc, u2 endPc, u2 handlerPc, u2 catchType);


    u2 getStartPc() const;

    void setStartPc(u2 startPc);

    u2 getEndPc() const;

    void setEndPc(u2 endPc);

    u2 getHandlerPc() const;

    void setHandlerPc(u2 handlerPc);

    u2 getCatchType() const;

    void setCatchType(u2 catchType);

    virtual ~Exception_table();

private:
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
};


#endif //JVM_EXCEPTION_TABLE_H
