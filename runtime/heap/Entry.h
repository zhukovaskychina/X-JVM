//
// Created by zhukovasky on 2020/8/17.
//

#ifndef JAVAP_ENTRY_H
#define JAVAP_ENTRY_H

#include "iostream"
#include "../../common/Internal.h"
#include "../../lang/ByteQueue.h"

namespace Runtime{
    namespace Heap{
        class Entry {
        public:

            virtual Entry* readClass(std::string className)=0;
            static Entry* newEntry(std::string cpOption);

            Lang::ByteQueue *getByteQueue() const;

            void setByteQueue(Lang::ByteQueue *byteQueue);

        private:

            Lang::ByteQueue *byteQueue;
        };

        class CompositeEntry:public Entry{
        public:
            CompositeEntry();

            Entry *readClass(std::string className) override;

            virtual ~CompositeEntry();

        private:

        };

        //此处参考了jvm.go
        class DirEntry:public Entry{
        public:
            DirEntry(std::string path);

            Entry *readClass(std::string className) override;

            virtual ~DirEntry();

        private:
            std::string absDir;
        };
        class ZipEntry:public Entry{
        public:
            ZipEntry(std::string path);

            ZipEntry();

            Entry *readClass(std::string className) override;

            //      Lang::ByteQueue* readClass(std::string className);

        private:
            std::string absDir;
        };


        class WildcardEntry:public Entry{
        public:
            WildcardEntry(std::string jreLibPath);

            WildcardEntry();

            Entry *readClass(std::string className) override;


        private:
            std::string jreLibPath;
        };

    }
}



#endif //JAVAP_ENTRY_H
