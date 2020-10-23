//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_BOOTSTRAPMETHOD_H
#define JAVAP_BOOTSTRAPMETHOD_H

#include "../../common/Internal.h"

namespace ClassFile{
    class BootstrapMethod {
    public:
        BootstrapMethod();

        virtual ~BootstrapMethod();

        u2 getBootstrapMethodRef() const;

        void setBootstrapMethodRef(u2 bootstrapMethodRef);

        u2 getNumBootstrapArguments() const;

        void setNumBootstrapArguments(u2 numBootstrapArguments);

        u2* getBootstrapMethodArguments() const;

        void setBootstrapMethodArguments(u2* bootstrapMethodArguments);

    private:
        u2 bootstrapMethodRef;
        u2 numBootstrapArguments;
        u2* bootstrapMethodArguments;
    };
}




#endif //JAVAP_BOOTSTRAPMETHOD_H
