//
// Created by zhukovasky on 2020/4/26.
//

#include "Exception_table.h"

Exception_table::Exception_table() {}

Exception_table::~Exception_table() {

}

Exception_table::Exception_table(u2 startPc, u2 endPc, u2 handlerPc, u2 catchType) : start_pc(startPc), end_pc(endPc),
                                                                                     handler_pc(handlerPc),
                                                                                     catch_type(catchType) {
    this->setStartPc(startPc);
    this->setHandlerPc(handlerPc);
    this->setEndPc(endPc);
    this->setCatchType(catchType);
}

u2 Exception_table::getStartPc() const {
    return start_pc;
}

void Exception_table::setStartPc(u2 startPc) {
    start_pc = startPc;
}

u2 Exception_table::getEndPc() const {
    return end_pc;
}

void Exception_table::setEndPc(u2 endPc) {
    end_pc = endPc;
}

u2 Exception_table::getHandlerPc() const {
    return handler_pc;
}

void Exception_table::setHandlerPc(u2 handlerPc) {
    handler_pc = handlerPc;
}

u2 Exception_table::getCatchType() const {
    return catch_type;
}

void Exception_table::setCatchType(u2 catchType) {
    catch_type = catchType;
}
