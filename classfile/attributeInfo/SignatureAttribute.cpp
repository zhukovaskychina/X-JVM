//
// Created by zhukovasky on 2020/4/26.
//

#include "SignatureAttribute.h"

ClassFile::SignatureAttribute::SignatureAttribute() {}

ClassFile::SignatureAttribute::~SignatureAttribute() {

}

u2 ClassFile::SignatureAttribute::getSignatureIndex() const {
    return signature_index;
}

void ClassFile::SignatureAttribute::setSignatureIndex(u2 signatureIndex) {
    signature_index = signatureIndex;
}

void ClassFile::SignatureAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::SignatureAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::SignatureAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::SignatureAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::SignatureAttribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::SignatureAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
