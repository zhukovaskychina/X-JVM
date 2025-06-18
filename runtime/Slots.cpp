//
// Created by zhukovasky on 2020/8/11.
//

#include <cstddef>
#include "Slots.h"
#include "../utils/PlatformCompat.h"
namespace Runtime{
    Slots::Slots(long nums, Object *refs) : nums(nums), refs(refs) {
        this->nums=nums;
        this->refs=refs;
    }
    Slots::Slots() {
        this->doubleValue= NULL;
    }
    Slots::~Slots() {
        delete this->refs;
    }

    int Slots::getNums() const {

        return nums;
    }

    void Slots::setNums(int nums) {
        this->type="int";
        this->nums = nums;
    }

    Object *Slots::getRefs() const {
        return refs;
    }

    void Slots::setRefs(Object *refs) {
        this->type="ref";
        this->refs = refs;
    }

    float Slots::getFloatValue() const {
        return floatValue;
    }

    void Slots::setFloatValue(float floatValue) {
        this->type="float";
         this->floatValue = floatValue;
    }

    double Slots::getDoubleValue() const {
        return doubleValue;
    }

    void Slots::setDoubleValue(double doubleValue) {
        this->type="double";
        this->doubleValue = doubleValue;
    }

    long Slots::getLongValue() const {
        return longValue;
    }

    void Slots::setLongValue(long longValue) {
        this->type="long";
        Slots::longValue = longValue;
    }

    std::string Slots::toString() {
#ifdef _MSC_VER
        return std::string(this->type);
#else
        return std::__cxx11::string(this->type);
#endif
    }

    uint8_t Slots::getByteValue() const {
        return byteValue;
    }

    void Slots::setByteValue(uint8_t byteValue) {
        this->type="byte";
        Slots::byteValue = byteValue;
    }

    uint16_t Slots::getShortValue() const {
        return shortValue;
    }

    void Slots::setShortValue(uint16_t shortValue) {
        this->type="short";
        Slots::shortValue = shortValue;
    }

}


