//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CLASSESSINNER_H
#define JVM_CLASSESSINNER_H


#include "../../common/Internal.h"

class ClassessInner {
public:
    ClassessInner();

    virtual ~ClassessInner();

    u2 getInnerClassInfoIndex() const;

    void setInnerClassInfoIndex(u2 innerClassInfoIndex);

    u2 getPuterClassInfoIndex() const;

    void setPuterClassInfoIndex(u2 puterClassInfoIndex);

    u2 getInnerNameIndex() const;

    void setInnerNameIndex(u2 innerNameIndex);

    u2 getInnerClassAccessFlags() const;

    void setInnerClassAccessFlags(u2 innerClassAccessFlags);

public:

private:
    u2 inner_class_info_index;
    u2 puter_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
};


#endif //JVM_CLASSESSINNER_H
