//
// Created by zhukovasky on 2020/8/13.
//

#include "ByteCodeReader.h"
namespace Instruction{
    ByteCodeReader::ByteCodeReader() {}

    ByteCodeReader::~ByteCodeReader() {
        delete this->code;
    }

    ByteCodeReader::ByteCodeReader(int pc, u1 *code) : pc(pc), code(code) {
        this->code=code;
        this->pc=pc;
    }

    ByteCodeReader::ByteCodeReader(u1 *code) : code(code) {
        this->pc=0;
        this->code=code;
    }

    int ByteCodeReader::getPc() const {
        return pc;
    }

    void ByteCodeReader::setPc(int pc) {
        ByteCodeReader::pc = pc;
    }

    u1 *ByteCodeReader::getCode() const {
        return code;
    }

    void ByteCodeReader::setCode(u1 *code) {
        ByteCodeReader::code = code;
    }

    u1 ByteCodeReader::readU1() {
        int position=this->pc;
        this->pc++;
        return this->code[position];
    }

    u2 ByteCodeReader::readU2() {
        u1 first=this->readU1();
        u1 second=this->readU1();

        return first<<8|second;
    }

    u4 ByteCodeReader::read4() {
        u1 first=this->readU1();
        u1 second=this->readU1();
        u1 third=this->readU1();
        u1 fourth=this->readU1();
        return (first<<24)|(second<<16)|(third<<8)|fourth;
    }

    void ByteCodeReader::reset(u1 *code, int pc) {
        this->code=code;
        this->pc=pc;
    }

}