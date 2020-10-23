//
// Created by zhukovasky on 2020/8/6.
//

#ifndef JAVAP_ANNOTATION_H
#define JAVAP_ANNOTATION_H

#include "ElementValuePairs.h"
#include <list>
namespace ClassFile{
    class Annotation{
    public:
        Annotation();

        virtual ~Annotation();

        u2 getTypeIndex() const;

        void setTypeIndex(u2 typeIndex);

        u2 getNumElementValuePairs() const;

        void setNumElementValuePairs(u2 numElementValuePairs);

        ElementValuePairs *getElementValuePairs() const;

        void setElementValuePairs(ElementValuePairs *elementValuePairs);

        const list<ElementValuePairs *> &getElementValuePairsList() const;

        void setElementValuePairsList(const list<ElementValuePairs *> &elementValuePairsList);

    public:




    private:
        u2 type_index;
        u2 num_element_value_pairs;
        std::list<ElementValuePairs*> elementValuePairsList;
        ElementValuePairs *elementValuePairs;
    };
}



#endif //JAVAP_ANNOTATION_H
