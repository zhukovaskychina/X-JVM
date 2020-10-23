//
// Created by zhukovasky on 2020/4/22.
//

#include "ClassFile.h"
#include "../common/JvmEnums.h"
#include <cstring>
#include "Constant_Class.h"
#include "Constant_MethodRef.h"
#include "Constant_FieldRef.h"
#include "Constant_InterfaceMethodRef.h"
#include "Constant_String_info.h"
#include "Constant_Integer_info.h"
#include "Constant_Float_info.h"
#include "Constant_Long_info.h"
#include "Constant_Double_info.h"
#include "Constant_NameAndType_info.h"
#include "Constant_Utf8_info.h"
#include "Constant_MethodHandle_info.h"
#include "Constant_MethodType_info.h"
#include "Constant_InvokeDynamic_info.h"
#include "attributeInfo/Exception_table.h"
#include "attributeInfo/ConstantValue.h"
#include "attributeInfo/Code_attribute.h"
#include "attributeInfo/LineNumberTableAttribute.h"
#include "attributeInfo/ElementValue.h"
#include  "attributeInfo/LocalVariableTableAttribute.h"
#include  "attributeInfo/DeprecatedAttribute.h"

#include  "attributeInfo/SourcFileAttribute.h"
#include "attributeInfo/RuntimeVisibleAnnotations.h"
#include "attributeInfo/SyntheticAttribute.h"
#include "attributeInfo/BootstrapMethodAttribute.h"
#include "attributeInfo/MethodParameterAttributes.h"
#include "attributeInfo/Exceptions_attribute.h"
#include "attributeInfo/InnerClasses.h"
#include "attributeInfo/EnclosingMethdAttribute.h"
#include "attributeInfo/SignatureAttribute.h"
#include "attributeInfo/SourceDebugExtensionAttribute.h"
#include "attributeInfo/RuntimeInvisibleAnnotations.h"
#include "attributeInfo/RuntimeVisibleParameterAnnotationAttributes.h"
#include "attributeInfo/RuntimeInvisibleParameterAnnotationAttributes.h"
#include "attributeInfo/AnnotationDefaultAttribute.h"
#include "attributeInfo/StackMapTableAttributes.h"

namespace ClassFile{

    ClassFile::ClassFile() {
    }

    ClassFile::ClassFile(ByteQueue *byteQueue) : byteQueue(byteQueue) {
        this->byteQueue=byteQueue;
    }

    ClassFile::~ClassFile() {
        delete(this->byteQueue);
    }

    void ClassFile::readClassFile() {
        this->magicNumber=this->byteQueue->popU4();
        this->minorVersion=this->byteQueue->popU2();
        this->majorVersion=this->byteQueue->popU2();
        this->parseConstantsPools();
        this->accessFlags=this->byteQueue->popU2();
        this->thisClass=this->byteQueue->popU2();
        this->superClass=this->byteQueue->popU2();
        this->parseInterfaces();
        this->parseFields();
        this->parseMethods();
        this->parseAttributes();
        int size=this->byteQueue->size();
        if(size>0){
            cerr<<"代码不应该执行"<<endl;
        }
    }

    void ClassFile::readClassFile(Lang::ByteQueue *byteQueue) {
        this->byteQueue=byteQueue;
        this->magicNumber=this->byteQueue->popU4();
        this->minorVersion=this->byteQueue->popU2();
        this->majorVersion=this->byteQueue->popU2();
        this->parseConstantsPools();
        this->accessFlags=this->byteQueue->popU2();
        this->thisClass=this->byteQueue->popU2();
        this->superClass=this->byteQueue->popU2();
        this->parseInterfaces();
        this->parseFields();
        this->parseMethods();
        this->parseAttributes();
    }

    void ClassFile::parseConstantsPools() {

        this->constantPoolsCount=this->byteQueue->popU2();
        //读取常量池
        //构建常量池
        //

        ConstantPoolsList *constantPoolsList=new ConstantPoolsList();
        for (int i = 1; i <this->constantPoolsCount ; ++i) {
            auto tag=byteQueue->popU1();

            switch (tag){
                default:
                    cerr << "undefined constant pool type\n";
                    break;
                case CONSTANT_Class: {
                    Constant_Class* constantClassInfo = new Constant_Class();
                    auto nameIndex = byteQueue->popU2();
                    constantClassInfo->setNameIndex(nameIndex);
                    constantClassInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantClassInfo);
                    break;
                }
                case CONSTANT_MethodRef: {
                    Constant_MethodRef *constantMethodRef=new Constant_MethodRef();

                    auto classIndex = byteQueue->popU2();
                    auto nameAndTypeIndex = byteQueue->popU2();
                    constantMethodRef->setClassIndex(classIndex);
                    constantMethodRef->setNameAndTypeIndex(nameAndTypeIndex);
                    constantMethodRef->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantMethodRef);
                    break;
                }
                case CONSTANT_FieldRef: {
                    u2 classIndex = byteQueue->popU2();
                    u2 nameAndTypeIndex = byteQueue->popU2();
                    Constant_FieldRef *constantFieldRefInfo = new Constant_FieldRef();
                    constantFieldRefInfo->setNameAndTypeIndex(nameAndTypeIndex);
                    constantFieldRefInfo->setClassIndex(classIndex);
                    constantFieldRefInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantFieldRefInfo);
                    break;
                }
                case CONSTANT_InterfaceMethodRef: {
                    //CONSTANT_InterfaceMethodRef
                    Constant_InterfaceMethodRef *constantInterfaceMethod = new Constant_InterfaceMethodRef();
                    auto classIndex = byteQueue->popU2();
                    auto nameAndTypeIndex = byteQueue->popU2();
                    constantInterfaceMethod->setClassIndex(classIndex);
                    constantInterfaceMethod->setNameAndTypeIndex(nameAndTypeIndex);
                    constantInterfaceMethod->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantInterfaceMethod);
                    break;
                }
                case CONSTANT_String: {
                    auto stringIndex = byteQueue->popU2();
                    Constant_StringInfo *constantStringInfo = new Constant_StringInfo();
                    constantStringInfo->setStringIndex(stringIndex);
                    constantStringInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantStringInfo);
                    break;
                }
                case CONSTANT_Integer: {
                    Constant_IntegerInfo *constantIntegerInfo = new Constant_IntegerInfo();
                    auto integerBytes = byteQueue->popU4();
                    constantIntegerInfo->setBytes(integerBytes);
                    constantIntegerInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantIntegerInfo);
                    break;
                }
                case CONSTANT_Float: {
                    auto floatBytes = byteQueue->popU4();
                    Constant_FloatInfo *constantFloatInfo = new Constant_FloatInfo();
                    constantFloatInfo->setBytes(floatBytes);
                    constantFloatInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantFloatInfo);
                    break;
                }
                case CONSTANT_Long: {

                    auto highBytes = byteQueue->popU4();
                    auto lowBytes = byteQueue->popU4();

                    Constant_LongInfo *constantLongInfo = new Constant_LongInfo();
                    constantLongInfo->setHighBytes(highBytes);
                    constantLongInfo->setLowBytes(lowBytes);
                    constantLongInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantLongInfo);
                    constantPoolsList->setConstantPoolsInstance(i+1, nullptr);
                    break;
                }
                case CONSTANT_Double: {
                    auto highBytes = byteQueue->popU4();
                    auto lowBytes = byteQueue->popU4();
                    Constant_DoubleInfo *constantDoubleInfo = new Constant_DoubleInfo();
                    constantDoubleInfo->setHighBytes(highBytes);
                    constantDoubleInfo->setLowBytes(lowBytes);
                    constantDoubleInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantDoubleInfo);
                    constantPoolsList->setConstantPoolsInstance(i+1, nullptr);
                    break;
                }
                case CONSTANT_NameAndType: {
                    Constant_NameAndTypeInfo *constantNameAndTypeInfo = new Constant_NameAndTypeInfo();
                    auto nameIndex = byteQueue->popU2();
                    auto descriptorIndex =byteQueue->popU2();
                    constantNameAndTypeInfo->setNameIndex(nameIndex);
                    constantNameAndTypeInfo->setDescriptorIndex(descriptorIndex);
                    constantNameAndTypeInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantNameAndTypeInfo);
                    break;
                }
                case CONSTANT_Utf8: {

                    auto length = byteQueue->popU2();
                    Constant_Utf8Info *constantUtf8Info = new Constant_Utf8Info();

                    u1 *bytes = new u1[length + 1];
                    for (int j = 0; j < length; ++j) {
                        bytes[j] = byteQueue->popU1();
                    }
                    bytes[length] = '\0';
                    constantUtf8Info->setBytes(bytes);
                    constantUtf8Info->setLength(length);
                    constantUtf8Info->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantUtf8Info);

                    break;
                }
                case CONSTANT_MethodHandle: {
                    auto referenceKind = byteQueue->popU1();
                    auto referenceIndex = byteQueue->popU2();
                    Constant_MethodHandleInfo *constantMethodHandleInfo = new Constant_MethodHandleInfo();
                    constantMethodHandleInfo->setReferenceIndex(referenceIndex);
                    constantMethodHandleInfo->setReferenceKind(referenceKind);
                    constantMethodHandleInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantMethodHandleInfo);
                    break;
                }
                case CONSTANT_MethodType: {
                    auto descriptorIndex = byteQueue->popU2();
                    Constant_MethodTypeInfo *constantMethodTypeInfo = new Constant_MethodTypeInfo();
                    constantMethodTypeInfo->setDescriptorIndex(descriptorIndex);
                    constantMethodTypeInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantMethodTypeInfo);
                    break;
                }
                case CONSTANT_InvokeDynamic: {
                    auto bootstrapMethodAttrIndex = byteQueue->popU2();
                    auto nameAndTypeIndex = byteQueue->popU2();
                    Constant_InvokeDynamicInfo *constantInvokeDynamicInfo = new Constant_InvokeDynamicInfo();
                    constantInvokeDynamicInfo->setNameAndTypeIndex(nameAndTypeIndex);
                    constantInvokeDynamicInfo->setBootstrapMethodAttrIndex(bootstrapMethodAttrIndex);
                    constantInvokeDynamicInfo->setTag(tag);
                    constantPoolsList->setConstantPoolsInstance(i,constantInvokeDynamicInfo);

                    break;
                }
            }
            if(tag==5||tag==6){
                i=i+1;
            }
        }

        this->constantPoolsList=constantPoolsList;
        this->constantPoolsList;
    }

    void ClassFile::parseInterfaces() {
        //解析interfaces

        auto interfaceCount = this->byteQueue->popU2();
        u2* interfaces[interfaceCount];
        for (int m = 0; m < interfaceCount; ++m) {
            auto interfaceIndex=this->byteQueue->popU2();
            auto constantClass=this->constantPoolsList->getIndex(interfaceIndex);
            auto interface_class_index= reinterpret_cast<Constant_Class*>(&constantClass)->getNameIndex();
                    //dynamic_cast<Constant_Class*>(constantClass)->getNameIndex();
         //   auto interfaceName=dynamic_cast<Constant_Utf8Info*>(constantPools[interface_class_index])->getBytes();
         //   auto interfaceName= reinterpret_cast<Constant_Utf8Info*>(this->constantPoolsList->getIndex(interface_class_index))->getBytes();

            this->interfaces.push_back(interfaceIndex);
        }
        this->interfaceCounts=interfaceCount;
//        this->interfaces=*interfaces;

    }

    void ClassFile::parseFields(){
        //解析属性
        u2 fieldCount=this->byteQueue->popU2();
        this->fieldsCount=fieldCount;
        for (int k = 0; k <fieldCount; ++k) {
            FieldInfo *fieldInfo=new FieldInfo();
            fieldInfo->setAccessFlags(byteQueue->popU2());
            fieldInfo->setNameIndex(byteQueue->popU2());
            fieldInfo->setDescriptorIndex(byteQueue->popU2());
            u2 fieldAttributesCount=byteQueue->popU2();
            fieldInfo->setAttributesCount(fieldAttributesCount);
            std::list<AttributesInfo*> fieldInfoAttributesList;
            for (int i = 0; i < fieldAttributesCount; ++i) {
                fieldInfoAttributesList.push_back(this->parseAttribute());
            }
            fieldInfo->setAttributesInfoList(fieldInfoAttributesList);
            this->fieldInfoList.push_back(fieldInfo);
        }
       int size=this->fieldInfoList.size();

    }



    u4 ClassFile::getMagicNumber() const {
        return magicNumber;
    }

    void ClassFile::setMagicNumber(u4 magicNumber) {
        ClassFile::magicNumber = magicNumber;
    }

    u2 ClassFile::getMinorVersion() const {
        return minorVersion;
    }

    void ClassFile::setMinorVersion(u2 minorVersion) {
        ClassFile::minorVersion = minorVersion;
    }

    u2 ClassFile::getMajorVersion() const {
        return majorVersion;
    }

    void ClassFile::setMajorVersion(u2 majorVersion) {
        ClassFile::majorVersion = majorVersion;
    }

    u2 ClassFile::getConstantPoolsCount() const {
        return constantPoolsCount;
    }

    void ClassFile::setConstantPoolsCount(u2 constantPoolsCount) {
        ClassFile::constantPoolsCount = constantPoolsCount;
    }

    ConstantPoolsList *ClassFile::getConstantPools() const {
        return this->constantPoolsList;
    }



    u2 ClassFile::getAccessFlags() const {
        return accessFlags;
    }

    void ClassFile::setAccessFlags(u2 accessFlags) {
        ClassFile::accessFlags = accessFlags;
    }

    u2 ClassFile::getThisClass() const {
        return thisClass;
    }

    void ClassFile::setThisClass(u2 thisClass) {
        ClassFile::thisClass = thisClass;
    }

    u2 ClassFile::getSuperClass() const {
        return superClass;
    }

    void ClassFile::setSuperClass(u2 superClass) {
        ClassFile::superClass = superClass;
    }

    u2 ClassFile::getInterfaceCounts() const {
        return interfaceCounts;
    }

    void ClassFile::setInterfaceCounts(u2 interfaceCounts) {
        ClassFile::interfaceCounts = interfaceCounts;
    }



    u2 ClassFile::getFieldsCount() const {
        return fieldsCount;
    }

    void ClassFile::setFieldsCount(u2 fieldsCount) {
        ClassFile::fieldsCount = fieldsCount;
    }

    FieldInfo *ClassFile::getFieldInfo() const {
        return fieldInfo;
    }

    void ClassFile::setFieldInfo(FieldInfo *fieldInfo) {
        ClassFile::fieldInfo = fieldInfo;
    }

    u2 ClassFile::getMethodCount() const {
        return methodCount;
    }

    void ClassFile::setMethodCount(u2 methodCount) {
        ClassFile::methodCount = methodCount;
    }

    MethodInfo *ClassFile::getMethodInfo() const {
        return methodInfo;
    }

    void ClassFile::setMethodInfo(MethodInfo *methodInfo) {
        ClassFile::methodInfo = methodInfo;
    }

    u2 ClassFile::getAttributesCount() const {
        return attributesCount;
    }

    void ClassFile::setAttributesCount(u2 attributesCount) {
        ClassFile::attributesCount = attributesCount;
    }

    AttributesInfo *ClassFile::getAttributeInfo() const {
        return attributeInfo;
    }

    void ClassFile::setAttributeInfo(AttributesInfo *attributeInfo) {
        ClassFile::attributeInfo = attributeInfo;
    }

    std::string ClassFile::getThisClassName() {
        Constant_Class *constantClass= reinterpret_cast<Constant_Class*>(this->constantPoolsList->getIndex(this->thisClass));
        std::string thisClassName=this->getUTF8(constantClass->getNameIndex());
        return thisClassName;
    }

    std::string ClassFile::getSuperClassName() {
        if(this->superClass>0){
            Constant_Class *constantClass= reinterpret_cast<Constant_Class*>(this->constantPoolsList->getIndex(this->superClass));
            std::string thisClassName=this->getUTF8(constantClass->getNameIndex());
            return thisClassName;
        }

        return "";
    }

    const list<MethodInfo *> &ClassFile::getMethodInfoList() const {
        return methodInfoList;
    }

    void ClassFile::setMethodInfoList(const list<MethodInfo *> &methodInfoList) {
        ClassFile::methodInfoList = methodInfoList;
    }

    void ClassFile::parseMethods() {
        u2 methodCount=this->byteQueue->popU2();

        this->methodCount=methodCount;
        for (int i = 0; i <methodCount ; ++i) {
            MethodInfo *methodInfoIter=new MethodInfo();
            methodInfoIter->setAccessFlags(this->byteQueue->popU2());
            methodInfoIter->setNameIndex(this->byteQueue->popU2());
            methodInfoIter->setDescriptorIndex(this->byteQueue->popU2());
            u2 attributesCount=this->byteQueue->popU2();
            methodInfoIter->setAttributesCount(attributesCount);

            std::list<AttributesInfo*> attributesInfoList;
            for (int j = 0; j < attributesCount; ++j) {
                AttributesInfo *iter=this->parseAttribute();
                attributesInfoList.push_back(iter);
            }
            methodInfoIter->setAttributesInfoList(attributesInfoList);
            this->methodInfoList.push_back(methodInfoIter);
        }
        this->methodInfoList.size();
    }

    void ClassFile::parseAttributes() {
        u2 attributesCount=this->byteQueue->popU2();
        this->attributesCount=attributesCount;

        for (int i = 0; i <attributesCount ; ++i) {
            AttributesInfo *attributesInfoIter=this->parseAttribute();
            if(attributesInfoIter== nullptr){
                cerr<<"出现异常了"<<endl;
            }
            this->attributeInfoList.push_back(attributesInfoIter);
        }
        int size=this->attributeInfoList.size();

    }

    AttributesInfo *ClassFile::parseAttribute() {
        const auto attribute_name_index = byteQueue->popU2();

//        if (typeid(this->constantPools[attribute_name_index]) != typeid(Constant_Utf8Info)) {
//            return nullptr;
//        }

        Constant_Utf8Info *constantUtf8Info = dynamic_cast<Constant_Utf8Info *>(this->constantPoolsList->getIndex(attribute_name_index));

        u1 *name = constantUtf8Info->getBytes();
        AttributesInfo *attributeInfo = nullptr;
        const char *nameChar = (char *) name;
        std::string namestr = nameChar;
        if (strcmp("Code", nameChar) == 0) {
            attributeInfo = new Code_attribute();
            Code_attribute *codeAttribute = dynamic_cast<Code_attribute *>(attributeInfo);
            codeAttribute->setAttributeNameIndex(attribute_name_index);
            codeAttribute->setAttributeLength(byteQueue->popU4());

            u2 maxStack =byteQueue->popU2();
            u2 maxLocals =byteQueue->popU2();
            u4 code_length = byteQueue->popU4();
            u1 *code = getU1Content(code_length, byteQueue);
            u2 exception_table_length = byteQueue->popU2();
            std::list<Exception_table*> exceptionTableList=getExceptionTablesList(exception_table_length,byteQueue);
            codeAttribute->setMaxStack(maxStack);
            codeAttribute->setMaxLocals(maxLocals);
            codeAttribute->setCode(code);
            codeAttribute->setExceptionTableLength(exception_table_length);
            codeAttribute->setExceptionTableList(exceptionTableList);
            u2 attributesCount = byteQueue->popU2();
            std::list<AttributesInfo*> attributesInfoList;
            for (int i = 0; i < attributesCount; ++i) {
                AttributesInfo *attributeTemp = this->parseAttribute();
                attributesInfoList.push_back(attributeTemp);
            }
            codeAttribute->setTags(nameChar);
            codeAttribute->setAttributesInfoList(attributesInfoList);
            attributeInfo = reinterpret_cast<AttributesInfo *>(codeAttribute);
        }
        if (strcmp("ConstantValue", nameChar) == 0) {
            attributeInfo = new ConstantValue();
            auto attributeInfoAttribute = dynamic_cast<ConstantValue *>(attributeInfo);
            attributeInfoAttribute->setAttributeNameIndex(attribute_name_index);
            auto length=byteQueue->popU4();
            attributeInfoAttribute->setAttributeLength(length);
            auto constantValueIndex = byteQueue->popU2();
            attributeInfoAttribute->setConstantvalueIndex(constantValueIndex);
            attributeInfoAttribute->setTags(nameChar);
            dynamic_cast<AttributesInfo *>(attributeInfoAttribute);
            attributeInfo = reinterpret_cast<AttributesInfo *>(attributeInfoAttribute);
        }
//
        if (strcmp("LineNumberTable", nameChar) == 0) {
            attributeInfo = new LineNumberTableAttribute();
            auto attribute_length = byteQueue->popU4();
            auto attributeInfoLine = dynamic_cast<LineNumberTableAttribute *>(attributeInfo);
            attributeInfoLine->setAttributeLength(attribute_length);
            auto line_number_table_length = byteQueue->popU2();
            attributeInfoLine->setLineNumberTableLength(line_number_table_length);
            std::list<LineNumberTable*> lineNumberTableEntriesList;
            for (int i = 0; i < line_number_table_length; ++i) {
                LineNumberTable *lineNumberTableEntriesIter = new LineNumberTable();
                lineNumberTableEntriesIter->setStart_pc(byteQueue->popU2());
                lineNumberTableEntriesIter->setLine_number(byteQueue->popU2());
                lineNumberTableEntriesList.push_back(lineNumberTableEntriesIter);

            }
            attributeInfoLine->setTags(nameChar);
            attributeInfoLine->setLineNumberTableList(lineNumberTableEntriesList);
            attributeInfo = reinterpret_cast<AttributesInfo *>(attributeInfoLine);
        }
        if (strcmp("LocalVariableTable", nameChar) == 0) {
            LocalVariableTableAttribute *localVariableTableAttribute = new LocalVariableTableAttribute();

            auto attribute_length = byteQueue->popU4();
            localVariableTableAttribute->setAttributeLength(attribute_length);
            localVariableTableAttribute->setAttributeNameIndex(attribute_name_index);
            auto local_variable_table_length = byteQueue->popU2();
            LocalVariableTable *localVariableTableEntries[local_variable_table_length];
            std::list<LocalVariableTable*> localVariableTableEntriesList;
            for (int i = 0; i < local_variable_table_length; ++i) {
                LocalVariableTable *localVariableTable = new LocalVariableTable();
                auto start_pc = byteQueue->popU2();
                auto length = byteQueue->popU2();
                auto name_index = byteQueue->popU2();
                auto descriptor_index =byteQueue->popU2();
                auto index = byteQueue->popU2();
                localVariableTable->setStart_pc(start_pc);
                localVariableTable->setName_index(name_index);
                localVariableTable->setDescriptor_index(descriptor_index);
                localVariableTable->setIndex(index);
                localVariableTable->setLength(length);
                localVariableTableEntriesList.push_back(localVariableTable);

            }
            localVariableTableAttribute->setLocalVariableTableList(localVariableTableEntriesList);
            localVariableTableAttribute->setTags(nameChar);
            //向上造型
            attributeInfo = reinterpret_cast<AttributesInfo *>(localVariableTableAttribute);
        }
        if (strcmp("Deprecated", nameChar) == 0) {
            DeprecatedAttribute *deprecatedAttribute = new DeprecatedAttribute();
            deprecatedAttribute->setAttributeNameIndex(attribute_name_index);
            auto deprecatedAttributeLength = byteQueue->popU4();
            deprecatedAttribute->setAttributeLength(deprecatedAttributeLength);
            deprecatedAttribute->setTags(nameChar);
            attributeInfo = reinterpret_cast<AttributesInfo *>(deprecatedAttribute);
        }
        if(strcmp("RuntimeVisibleAnnotations",nameChar)==0){
            RuntimeVisibleAnnotations *runtimeVisibleAnnotations=new RuntimeVisibleAnnotations();
            runtimeVisibleAnnotations->setAttributeNameIndex(attribute_name_index);
            auto attributeLength=byteQueue->popU4();
            runtimeVisibleAnnotations->setAttributeLength(attributeLength);
            auto num_annotations=byteQueue->popU2();
            std::list<Annotation*> annotationArrayList;
            for (int i = 0; i < num_annotations; ++i) {
                Annotation *annotations=new Annotation();
                auto type_index=byteQueue->popU2();
                auto num_element_value_pairs=byteQueue->popU2();
                annotations->setNumElementValuePairs(num_element_value_pairs);
                std::list<ElementValuePairs*> elementValuePairsList;
                for (int j = 0; j < num_element_value_pairs; ++j) {
                    ElementValuePairs* elementValuePairsIter=new ElementValuePairs();
                    elementValuePairsIter->setElementNameIndex(byteQueue->popU2());
                    ElementValue *elementValue=new ElementValue();
                    auto elementTag=byteQueue->popU2();
                    elementValue->setTags(elementTag);
                    elementValuePairsIter->setElementValue(*elementValue);
                    elementValuePairsList.push_back(elementValuePairsIter);
                }
                annotations->setElementValuePairsList(elementValuePairsList);
                annotations->setTypeIndex(type_index);
                annotations->setNumElementValuePairs(num_element_value_pairs);
                annotationArrayList.push_back(annotations);
            }
            runtimeVisibleAnnotations->setAnnotationsList(annotationArrayList);
            runtimeVisibleAnnotations->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo *>(runtimeVisibleAnnotations);
        }
        if(strcmp("BootstrapMethods",nameChar)==0){
            BootstrapMethodAttribute *bootstrapMethodAttribute=new BootstrapMethodAttribute();
            bootstrapMethodAttribute->setAttributeNameIndex(attribute_name_index);
            bootstrapMethodAttribute->setAttributeLength(byteQueue->popU4());

            u2 numBootstrapMethods=byteQueue->popU2();
            BootstrapMethod *bootstrapMethod[numBootstrapMethods];
            std::list<BootstrapMethod*> bootstrapMethodList;
            for (int i = 0; i <numBootstrapMethods ; ++i) {
                BootstrapMethod *bootstrapMethodIter=new BootstrapMethod;
                bootstrapMethodIter->setBootstrapMethodRef(byteQueue->popU2());
                u2 numbootstrapArguments=byteQueue->popU2();
                u2* bootstrapArgutments[numbootstrapArguments];
                for (int j = 0; j < numbootstrapArguments; ++j) {
                    bootstrapArgutments[j]= reinterpret_cast<u2 *>(byteQueue->popU2());
                }
                bootstrapMethodIter->setBootstrapMethodArguments(*bootstrapArgutments);
                bootstrapMethodList.push_back(bootstrapMethodIter);
            }
            bootstrapMethodAttribute->setNumBootstrapMethods(numBootstrapMethods);
            bootstrapMethodAttribute->setBootstrapMethodList(bootstrapMethodList);
            bootstrapMethodAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo *>(bootstrapMethodAttribute);
        }
        if(strcmp("Synthetic",nameChar)==0){
            SyntheticAttribute *syntheticAttribute=new SyntheticAttribute();
            syntheticAttribute->setAttributeNameIndex(attribute_name_index);
            auto syntheticAttributeLength = byteQueue->popU4();
            syntheticAttribute->setAttributeLength(syntheticAttributeLength);
            syntheticAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo *>(syntheticAttribute);
        }
        if(strcmp("SourceFile",nameChar)==0){
            SourceFileAttribute *sourceFileAttribute=new SourceFileAttribute();
            auto attributeLength=byteQueue->popU4();
            auto sourcefile_index=byteQueue->popU2();

            sourceFileAttribute->setAttributeLength(attributeLength);
            sourceFileAttribute->setAttributeNameIndex(sourcefile_index);
            sourceFileAttribute->setSourcefile_index(sourcefile_index);
            sourceFileAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(sourceFileAttribute);
        }
        if(strcmp("Exceptions",nameChar)==0){
            Exceptions_attribute *exceptionsAttribute=new Exceptions_attribute();
            exceptionsAttribute->setAttributeNameIndex(attribute_name_index);
            exceptionsAttribute->setAttributeLength(byteQueue->popU4());
            u2 numberOfExceptions=byteQueue->popU2();
            u2* exceptionIndexTable=new u2[numberOfExceptions];
            for (int i = 0; i < numberOfExceptions; ++i) {
                exceptionIndexTable[i]=(byteQueue->popU2());
            }
            exceptionsAttribute->setNumberOfExceptions(numberOfExceptions);
            exceptionsAttribute->setExceptionIndexTable(exceptionIndexTable);
            exceptionsAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(exceptionsAttribute);
        }
        if(strcmp("InnerClasses",nameChar)==0){
            InnerClasses *innerClass=new InnerClasses();
            innerClass->setAttributeNameIndex(attribute_name_index);
            innerClass->setAttributeLength(byteQueue->popU4());

            u2 numOfInnerClasses=byteQueue->popU2();
            ClassessInner* classessInner[numOfInnerClasses];
            for (int i = 0; i < numOfInnerClasses; ++i) {
                ClassessInner *classessInnerIter=new ClassessInner();
                classessInnerIter->setInnerClassAccessFlags(byteQueue->popU2());
                classessInnerIter->setInnerClassInfoIndex(byteQueue->popU2());
                classessInnerIter->setInnerNameIndex(byteQueue->popU2());
                classessInnerIter->setPuterClassInfoIndex(byteQueue->popU2());
                classessInner[i]=classessInnerIter;
            }
            innerClass->setNumberOfClasses(numOfInnerClasses);
            innerClass->setClassess(*classessInner);
            innerClass->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(innerClass);
        }
        if(strcmp("EnclosingMethod",nameChar)==0){
            EnclosingMethodAttribute *enclosingMethod=new EnclosingMethodAttribute();
            enclosingMethod->setAttributeNameIndex(attribute_name_index);
            enclosingMethod->setAttributeLength(byteQueue->popU4());
            enclosingMethod->setClassIndex(byteQueue->popU2());
            enclosingMethod->setMethodIndex(byteQueue->popU2());
            enclosingMethod->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(enclosingMethod);
        }
        if(strcmp("Signature",nameChar)==0){
            SignatureAttribute *signatureAttribute=new SignatureAttribute();
            signatureAttribute->setAttributeLength(byteQueue->popU4());
            signatureAttribute->setAttributeNameIndex(attribute_name_index);
            signatureAttribute->setSignatureIndex(byteQueue->popU2());
            signatureAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(signatureAttribute);
        }
        if(strcmp("SourceDebugExtension",nameChar)==0){
            SourceDebugExtensionAttribute *sourceDebugExtensionAttribute=new SourceDebugExtensionAttribute();

            sourceDebugExtensionAttribute->setAttributeNameIndex(attribute_name_index);
            auto attributeLength=byteQueue->popU4();
            sourceDebugExtensionAttribute->setAttributeLength(attributeLength);
            u1 *sourceDebugExtensions[attributeLength];
            for (int i = 0; i < attributeLength; ++i) {
                *sourceDebugExtensions[i]=byteQueue->popU1();
            }
            sourceDebugExtensionAttribute->setDebugExtension(*sourceDebugExtensions);
            sourceDebugExtensionAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(sourceDebugExtensionAttribute);
        }
        if(strcmp("LocalVariableTypeTable",nameChar)==0){
            LocalVariableTableAttribute *localVariableTableAttribute=new LocalVariableTableAttribute();
            localVariableTableAttribute->setAttributeLength(byteQueue->popU4());
            localVariableTableAttribute->setAttributeNameIndex(attribute_name_index);
            u2 localVariableTableLength=byteQueue->popU2();
            localVariableTableAttribute->setLocal_variable_table_length(localVariableTableLength);
            LocalVariableTable* localVariableTable[localVariableTableLength];
            for (int i = 0; i < localVariableTableLength; ++i) {
                LocalVariableTable *localVariableTableIter=new LocalVariableTable();
                localVariableTableIter->setStart_pc(byteQueue->popU2());
                localVariableTableIter->setLength(byteQueue->popU2());
                localVariableTableIter->setName_index(byteQueue->popU2());
                localVariableTableIter->setDescriptor_index(byteQueue->popU2());
                localVariableTableIter->setIndex(byteQueue->popU2());
                localVariableTable[i]=localVariableTableIter;
            }
            localVariableTableAttribute->setLocalVariableTable(*localVariableTable);
            localVariableTableAttribute->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(localVariableTableAttribute);
        }

        if(strcmp("RuntimeInvisibleAnnotations",nameChar)==0){
            RuntimeInvisibleAnnotations *runtimeInvisibleAnnotations=new RuntimeInvisibleAnnotations();
            runtimeInvisibleAnnotations->setAttributeNameIndex(attribute_name_index);
            runtimeInvisibleAnnotations->setAttributeLength(byteQueue->popU4());
            u2 num_annotations=byteQueue->popU2();
            runtimeInvisibleAnnotations->setNumAnnotations(num_annotations);
            Annotation* annotations[num_annotations];
            for (int i = 0; i <num_annotations ; ++i) {
                Annotation *annotation=new Annotation();
                annotation->setTypeIndex(byteQueue->popU2());
                u2 numElementValuePairs=byteQueue->popU2();
                annotation->setNumElementValuePairs(numElementValuePairs);
                ElementValuePairs* elementValuePairsArrays[numElementValuePairs];
                for (int j = 0; j < numElementValuePairs; ++j) {
                    ElementValuePairs *elementValuePairs=new ElementValuePairs();
                    elementValuePairs->setElementNameIndex(byteQueue->popU2());
                    ElementValue *elementValue=new ElementValue();
                    elementValue->setTags(byteQueue->popU1());
                    elementValuePairs->setElementValue(*elementValue);
                    elementValuePairsArrays[j]=elementValuePairs;
                }
                annotation->setElementValuePairs(*elementValuePairsArrays);
                annotations[i]=annotation;
                delete(annotation);
            }
            runtimeInvisibleAnnotations->setAnnotations(*annotations);
            runtimeInvisibleAnnotations->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(runtimeInvisibleAnnotations);
//            delete(runtimeInvisibleAnnotations);
//            delete(*annotations);
        }

        if(strcmp("RuntimeVisibleParameterAnnotations",nameChar)==0){
            RuntimeVisibleParameterAnnotationAttributes *runtimeVisibleParameterAnnotations=new RuntimeVisibleParameterAnnotationAttributes();
            runtimeVisibleParameterAnnotations->setAttributeNameIndex(attribute_name_index);
            runtimeVisibleParameterAnnotations->setAttributeLength(byteQueue->popU4());
            u1 numParameters=byteQueue->popU1();
            runtimeVisibleParameterAnnotations->setNumParameters(numParameters);
            ParameterAnnotations* parameterAnnotations[numParameters];
            for (int i = 0; i < numParameters; ++i) {
                ParameterAnnotations *parameterAnnotationsIter=new ParameterAnnotations();
                u2 numAnnotations=byteQueue->popU2();
                parameterAnnotationsIter->setNumAnnotations(numAnnotations);
                Annotation* annotations[numAnnotations];
                for (int i = 0; i <numAnnotations ; ++i) {
                    Annotation *annotation=new Annotation();
                    annotation->setTypeIndex(byteQueue->popU2());
                    u2 numElementValuePairs=byteQueue->popU2();
                    annotation->setNumElementValuePairs(numElementValuePairs);
                    ElementValuePairs* elementValuePairsArrays[numElementValuePairs];
                    for (int j = 0; j < numElementValuePairs; ++j) {
                        ElementValuePairs *elementValuePairs=new ElementValuePairs();
                        elementValuePairs->setElementNameIndex(byteQueue->popU2());
                        ElementValue *elementValue=new ElementValue();
                        elementValue->setTags(byteQueue->popU1());
                        elementValuePairs->setElementValue(*elementValue);
                        elementValuePairsArrays[j]=elementValuePairs;
                    }
                    annotation->setElementValuePairs(*elementValuePairsArrays);
                    annotations[i]=annotation;
                    delete(annotation);
                }
                parameterAnnotationsIter->setAnnotations(*annotations);
            }
            runtimeVisibleParameterAnnotations->setParameterAnnotations(*parameterAnnotations);
            runtimeVisibleParameterAnnotations->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(runtimeVisibleParameterAnnotations);
        }

        if(strcmp("RuntimeInvisibleParameterAnnotations",nameChar)==0){
            RuntimeInvisibleParameterAnnotationAttributes *runtimeVisibleParameterAnnotations=new RuntimeInvisibleParameterAnnotationAttributes();
            runtimeVisibleParameterAnnotations->setAttributeNameIndex(attribute_name_index);
            runtimeVisibleParameterAnnotations->setAttributeLength(byteQueue->popU4());
            u1 numParameters=byteQueue->popU1();
            runtimeVisibleParameterAnnotations->setNumParameters(numParameters);
            ParameterAnnotations* parameterAnnotations[numParameters];
            for (int i = 0; i < numParameters; ++i) {
                ParameterAnnotations *parameterAnnotationsIter=new ParameterAnnotations();
                u2 numAnnotations=byteQueue->popU2();
                parameterAnnotationsIter->setNumAnnotations(numAnnotations);
                Annotation* annotations[numAnnotations];
                for (int i = 0; i <numAnnotations ; ++i) {
                    Annotation *annotation=new Annotation();
                    annotation->setTypeIndex(byteQueue->popU2());
                    u2 numElementValuePairs=byteQueue->popU2();
                    annotation->setNumElementValuePairs(numElementValuePairs);
                    ElementValuePairs* elementValuePairsArrays[numElementValuePairs];
                    for (int j = 0; j < numElementValuePairs; ++j) {
                        ElementValuePairs *elementValuePairs=new ElementValuePairs();
                        elementValuePairs->setElementNameIndex(byteQueue->popU2());
                        ElementValue *elementValue=new ElementValue();
                        elementValue->setTags(byteQueue->popU1());
                        elementValuePairs->setElementValue(*elementValue);
                        elementValuePairsArrays[j]=elementValuePairs;
                    }
                    annotation->setElementValuePairs(*elementValuePairsArrays);
                    annotations[i]=annotation;
                    delete(annotation);
                }
                parameterAnnotationsIter->setAnnotations(*annotations);
            }
            runtimeVisibleParameterAnnotations->setParameterAnnotations(*parameterAnnotations);
            runtimeVisibleParameterAnnotations->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(runtimeVisibleParameterAnnotations);
        }
        if(strcmp("AnnotationDefault",nameChar)==0){
            AnnotationDefaultAttribute *annotationDefault=new AnnotationDefaultAttribute();
            annotationDefault->setAttributeNameIndex(attribute_name_index);
            annotationDefault->setAttributeLength(byteQueue->popU4());
            ElementValue *elementValue=new ElementValue();
            elementValue->setTags(byteQueue->popU1());
            annotationDefault->setDefaultValue(elementValue);
            annotationDefault->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(annotationDefault);
        }
        if(strcmp("StackMapTable",nameChar)==0){
            StackMapTableAttributes *stackMapTableAttributes=new StackMapTableAttributes();
            stackMapTableAttributes->setAttributeNameIndex(attribute_name_index);
            stackMapTableAttributes->setAttributeLength(byteQueue->popU4());
            u2 numberOfEntries=byteQueue->popU2();
            stackMapTableAttributes->setNumberOfEntries(numberOfEntries);
            std::list<StackMapFrame*> stackMapFrameArraysList;
            for (int i = 0; i < numberOfEntries; ++i) {
                StackMapFrame *stackMapFrame=new StackMapFrame();
                u1 tag=byteQueue->popU1();

                if(tag<=63){
                  //  cout<<tag<<endl;
                    //same_frame
                }
                if(tag<=127&&tag>=64){
                   // cout<<tag<<endl;
                    //same_locals_1_stack_item_frame
                    VerificationTypeInfo *verificationTypeInfo=new VerificationTypeInfo();
                    u1 vtag=byteQueue->popU1();
                    verificationTypeInfo->setTag(vtag);
                    if(vtag==7){
                        byteQueue->popU2();
                    }
                    if(vtag==8){
                        byteQueue->popU2();
                    }
                }
                if(tag>127&&tag<247){

                }
                if(tag>247&&tag<251){
                    byteQueue->popU2();
                }
                if(tag==251){
                    byteQueue->popU2();
                }
                if(tag>251&&tag<255){
                    int offsetDelta=byteQueue->popU2();
                    int size=tag-251;
                    for (int j = 0; j < size; ++j) {
                        u1 vtag=byteQueue->popU1();

                            if(vtag==7){
                                byteQueue->popU2();
                            }
                            if(vtag==8){
                                byteQueue->popU2();
                            }

                    }
                }
                if(tag==255){
                    int offsetDelta=byteQueue->popU2();
                    int numberOfLocals=byteQueue->popU2();
                    for (int j = 0; j < numberOfLocals; ++j) {
                        u1 vtag=byteQueue->popU1();
                        if(vtag==7){
                            byteQueue->popU2();
                        }
                        if(vtag==8){
                            byteQueue->popU2();
                        }
                    }
                    int numberOfStackItems=byteQueue->popU2();
                    for (int k = 0; k < numberOfStackItems; ++k) {
                        u1 vtag=byteQueue->popU1();
                        if(vtag==7){
                            byteQueue->popU2();
                        }
                        if(vtag==8){
                            byteQueue->popU2();
                        }
                    }
                }
                //  cout<<tag<<endl;
              //  stackMapFrame->setFrameType(tag);
               // u2 tags=byteQueue->popU2();
              //  cout<<tags<<endl;
                stackMapFrameArraysList.push_back(stackMapFrame);

            }
            stackMapTableAttributes->setStackMapFrameList(stackMapFrameArraysList);
            //stackMapTableAttributes->setStackMapFrame(*stackMapFrameArrays);
            stackMapTableAttributes->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(stackMapTableAttributes);
        }
        if(strcmp("RuntimeVisibleTypeAnnotations",nameChar)==0){

        }
        if(strcmp("RuntimeInvisibleTypeAnnotations",nameChar)==0){

        }
        if(strcmp("MethodParameters",nameChar)==0){
            MethodParameterAttributes *methodParameterAttributes=new MethodParameterAttributes();
            methodParameterAttributes->setAttributeNameIndex(attribute_name_index);
            methodParameterAttributes->setAttributeLength(byteQueue->popU4());
            u1 parametersCount=byteQueue->popU1();
            Parameters* parameters[parametersCount];
            for (int i = 0; i < parametersCount; ++i) {
                Parameters *parametersIter=new Parameters();
                parametersIter->setNameIndex(byteQueue->popU2());
                parametersIter->setAccessFlags(byteQueue->popU2());
            }
            methodParameterAttributes->setParameterCounts(parametersCount);
            methodParameterAttributes->setParameters(*parameters);
            methodParameterAttributes->setTags(nameChar);
            attributeInfo= reinterpret_cast<AttributesInfo*>(methodParameterAttributes);
        }
        attributeInfo->setTags(nameChar);
        return attributeInfo;

    }



    std::list<Exception_table *> ClassFile::getExceptionTablesList(u2 exception_table_length, ByteQueue *byteQueue) {
        std::list<Exception_table*> exceptionTableList;
        for (int i = 0; i < exception_table_length; ++i) {
            u2 start_pc = byteQueue->popU2();
            u2 end_pc = byteQueue->popU2();
            u2 handle_pc = byteQueue->popU2();
            u2 catch_type = byteQueue->popU2();
            Exception_table *exceptionTableTmp = new Exception_table();
            exceptionTableTmp->setCatchType(catch_type);
            exceptionTableTmp->setStartPc(start_pc);
            exceptionTableTmp->setEndPc(end_pc);
            exceptionTableTmp->setHandlerPc(handle_pc);
            exceptionTableList.push_back(exceptionTableTmp);
        }
        return exceptionTableList;
    }

    u1 *ClassFile::getU1Content(u4 length, ByteQueue *byteQueue) {
        u1 *u1Content = new u1[length];
        for (int i = 0; i < length; ++i) {
            u1Content[i] = byteQueue->popU1();
        }
        return u1Content;

    }

    std::string ClassFile::getUTF8(u2 index) {
       // ConstantPools &constantPools=this->constantPools[index];
        Constant_Utf8Info *constantUtf8Info= reinterpret_cast<Constant_Utf8Info*>(this->constantPoolsList->getIndex(index));

        std::string result=constantUtf8Info->toString();
    //    delete(constantUtf8Info);
        //delete(&constantPools);
        return result;
    }

    std::string ClassFile::getMethodName(u2 index) {
        std::string methodName=this->getUTF8(index);
        return methodName;
    }

    std::string ClassFile::getMethodDescriptor(u2 index) {
        return this->getUTF8(index);
    }

    std::string ClassFile::getMethodAccessFlags(u2 index) {
        std::string methodAccessFlags="";
        return std::__cxx11::string();
    }

    Methods* ClassFile::getMethods() {
        Methods *methods[this->methodCount];
        for (int i = 0; i < this->methodCount; ++i) {

            std::string methodName=this->getMethodName(this->methodInfo[i].getNameIndex());

        }
        return *methods;
    }

    std::vector<std::string> ClassFile::getInterfaceNames() {

        std::vector<string> interfaceNamesList;
        for (int i = 0; i < this->interfaceCounts; ++i) {
           // this->parseInterfaces()
            u2 index=this->interfaces[i];
            Constant_Class *constantClass= dynamic_cast<Constant_Class*>(this->constantPoolsList->getIndex(index));
            interfaceNamesList.push_back(this->getUTF8(constantClass->getNameIndex()));
        }
        return interfaceNamesList;
    }

    const list<FieldInfo *> &ClassFile::getFieldInfoList() const {
        return fieldInfoList;
    }

    void ClassFile::setFieldInfoList(const list<FieldInfo *> &fieldInfoList) {
        ClassFile::fieldInfoList = fieldInfoList;
    }
};