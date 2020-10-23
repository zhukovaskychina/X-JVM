//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_EXCEPTIONS_ATTRIBUTE_H
#define JVM_EXCEPTIONS_ATTRIBUTE_H


#include "../../common/Internal.h"
#include "../AttributesInfo.h"
namespace ClassFile{
    class Exceptions_attribute:public AttributesInfo {
    public:
        Exceptions_attribute();

        u2 getNumberOfExceptions() const;

        void setNumberOfExceptions(u2 numberOfExceptions);

        u2 *getExceptionIndexTable() const;

        void setExceptionIndexTable(u2 *exceptionIndexTable);

        virtual ~Exceptions_attribute();

        void setAttributeNameIndex(u2 attributeNameIndex) override;

        void setAttributeLength(u4 attributeLength) override;

        u2 getAttributeNameIndex() const override;

        u4 getAttributeLength() const override;

        const std::string getTags() const override;

        void setTags(std::string tags) override;

    public:

    private:
        u2 number_of_exceptions;
        u2 *exceptionIndexTable;
    };
}



#endif //JVM_EXCEPTIONS_ATTRIBUTE_H
