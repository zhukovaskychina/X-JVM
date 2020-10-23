//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_ELEMENTVALUEPAIRS_H
#define JAVAP_ELEMENTVALUEPAIRS_H


#include "ElementValue.h"

namespace ClassFile{
    class ElementValuePairs {
    public:
        ElementValuePairs();

        virtual ~ElementValuePairs();

        u2 getElementNameIndex() const;

        void setElementNameIndex(u2 elementNameIndex);

        const ElementValue &getElementValue() const;

        void setElementValue(const ElementValue &elementValue);

    private:
        u2 element_name_index;
        ElementValue elementValue;
    };
}




#endif //JAVAP_ELEMENTVALUEPAIRS_H
