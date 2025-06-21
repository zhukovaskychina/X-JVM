//
// Created by zhukovasky on 2020/8/11.
//

#ifndef JAVAP_SLOTS_H
#define JAVAP_SLOTS_H

#include "Object.h"
#include <iostream>
#include <cstdint>
namespace Runtime{

    class Object;
    class Slots {

    public:
        Slots();

        Slots(long nums, Object *refs);

        virtual ~Slots();

        int getNums() const;

        void setNums(int nums);

        Object *getRefs() const;

        void setRefs(Object *refs);

        float getFloatValue() const;

        void setFloatValue(float floatValue);

        double getDoubleValue() const;

        void setDoubleValue(double doubleValue);

        long getLongValue() const;

        void setLongValue(long longValue);

        std::string toString();

        std::string getSlotType();

        uint8_t getByteValue() const;

        void setByteValue(uint8_t byteValue);

        uint16_t getShortValue() const;

        void setShortValue(uint16_t shortValue);


    private:
        uint8_t byteValue;
        uint16_t shortValue;
        int nums;
        long longValue;
        float floatValue;
        double doubleValue;
        Object* refs;
        std::string type;
    };

}


#endif //JAVAP_SLOTS_H
