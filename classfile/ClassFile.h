//
// Created by zhukovasky on 2020/4/22.
//

#ifndef JVM_CLASSFILE_H
#define JVM_CLASSFILE_H

#include <list>
#include <array>
#include "../common/Internal.h"
#include "ConstantPools.h"

#include "FieldInfo.h"
#include "MethodInfo.h"
#include "AttributesInfo.h"
#include "../lang/ByteQueue.h"
#include "attributeInfo/Exception_table.h"

#include "Methods.h"
#include "ConstantPoolsList.h"

using namespace std;
namespace ClassFile {
    using namespace Lang;

    class ClassFile {
    public:
        ClassFile();

        ~ClassFile();

        ClassFile(ByteQueue *byteQueue);


        void readClassFile();
        //用于读取class字节码内容
        void readClassFile(ByteQueue *byteQueue);

        u4 getMagicNumber() const;

        void setMagicNumber(u4 magicNumber);

        u2 getMinorVersion() const;

        void setMinorVersion(u2 minorVersion);

        u2 getMajorVersion() const;

        void setMajorVersion(u2 majorVersion);

        u2 getConstantPoolsCount() const;

        void setConstantPoolsCount(u2 constantPoolsCount);

        ConstantPoolsList *getConstantPools() const;



        u2 getAccessFlags() const;

        void setAccessFlags(u2 accessFlags);

        u2 getThisClass() const;

        void setThisClass(u2 thisClass);

        u2 getSuperClass() const;

        void setSuperClass(u2 superClass);

        u2 getInterfaceCounts() const;

        void setInterfaceCounts(u2 interfaceCounts);


        u2 getFieldsCount() const;

        void setFieldsCount(u2 fieldsCount);

        FieldInfo *getFieldInfo() const;

        void setFieldInfo(FieldInfo *fieldInfo);

        u2 getMethodCount() const;

        void setMethodCount(u2 methodCount);

        MethodInfo *getMethodInfo() const;

        void setMethodInfo(MethodInfo *methodInfo);

        u2 getAttributesCount() const;

        void setAttributesCount(u2 attributesCount);

        AttributesInfo *getAttributeInfo() const;

        void setAttributeInfo(AttributesInfo *attributeInfo);

        std::string getUTF8(u2 index);


        std::string getMethodName(u2 index);
        //方法区获取方法描述
        std::string getMethodDescriptor(u2 index);

        std::string getMethodAccessFlags(u2 index);

        Methods* getMethods();

        std::string getThisClassName();

        std::string getSuperClassName();

        std::vector<std::string> getInterfaceNames();

        const list<MethodInfo *> &getMethodInfoList() const;

        void setMethodInfoList(const list<MethodInfo *> &methodInfoList);

        const list<FieldInfo *> &getFieldInfoList() const;

        void setFieldInfoList(const list<FieldInfo *> &fieldInfoList);

    private:

        ByteQueue *byteQueue;
        u4 magicNumber;
        u2 minorVersion;
        u2 majorVersion;
        u2 constantPoolsCount;

        ConstantPoolsList *constantPoolsList;
        u2 accessFlags;
        u2 thisClass;
        u2 superClass;
        u2 interfaceCounts;

        std::vector<u2> interfaces;
        u2 fieldsCount;
        FieldInfo *fieldInfo;
        std::list<FieldInfo*> fieldInfoList;
        u2 methodCount;
        MethodInfo *methodInfo;
        std::list<MethodInfo*> methodInfoList;
        u2 attributesCount;
        AttributesInfo *attributeInfo;
        std::list<AttributesInfo*> attributeInfoList;



        void parseConstantsPools();

        void parseInterfaces();

        void parseFields();

        void parseMethods();

        void parseAttributes();

        AttributesInfo *parseAttribute();

        Exception_table *getExceptionTables(u2 exception_table_length, ByteQueue *byteQueue);

        std::list<Exception_table*> getExceptionTablesList(u2 exception_table_length,ByteQueue *byteQueue);

        u1 *getU1Content(u4 length, ByteQueue *byteQueue);


    };
};

#endif //JVM_CLASSFILE_H
