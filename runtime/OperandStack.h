//
// Created by zhukovasky on 2020/8/11.
//

#ifndef JAVAP_OPERANDSTACK_H
#define JAVAP_OPERANDSTACK_H

#include <stack>
#include <vector>
#include "JavaType.h"
#include "iostream"
#include "Slots.h"

namespace Runtime{
    class OperandStack {

    public:

        static OperandStack* newOperandStack(int maxSize);

        void pushInt(int value);

        int popInt();

        void pushLong(long value);

        long popLong();

        void pushFloat(float value);

        float popFloat();

        void pushDouble(double value);

        double popDouble();

        void pushRef(Object* refs);

        Object* popObject();

        void pushBoolean(bool booleans);

        bool popBoolean();

        Slots* popSlots();

        void pushSlots(Slots* slots);

        Object* getRefFromTop(long index);
    private:

        std::vector<Slots*> innerSlotsList;
        int size;
        int maxSize;
    };
}



#endif //JAVAP_OPERANDSTACK_H
