//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_PARAMETERS_H
#define JAVAP_PARAMETERS_H


#include "../../common/Internal.h"

namespace ClassFile{
    class Parameters {
    public:
        Parameters();

        virtual ~Parameters();

        u2 getNameIndex() const;

        void setNameIndex(u2 nameIndex);

        u2 getAccessFlags() const;

        void setAccessFlags(u2 accessFlags);

    private:
        u2 name_index;
        u2 access_flags;
    };

}



#endif //JAVAP_PARAMETERS_H
