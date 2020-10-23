//
// Created by zhukovasky on 2020/8/30.
//

#ifndef JVM_LOCALVARIABLETABLES_H
#define JVM_LOCALVARIABLETABLES_H

#include <vector>
#include "../common/Internal.h"
#include "Slots.h"
#include "Object.h"

namespace Runtime{
    class LocalVariableTables {
    public:
        LocalVariableTables();

        LocalVariableTables(int maxLocals);

        int getInt(long index);

        void setInt(long index, int value);

        float getFloat(long index);

        void setFloat(long index,float value);

        long getLong(long index);

        void setLong(long index,long value);


        double getDouble(long index);

        void setDouble(long index,double value);

        Object* getRef(long index);

        void setRefs(long index,Object* refs);

        virtual ~LocalVariableTables();

        void setSlots(long index,Slots* slots);

        Slots* getSlots(long index);
    private:
        std::vector<Slots*> localVars;

        int maxLocals;
    };
}



#endif //JVM_LOCALVARIABLETABLES_H
