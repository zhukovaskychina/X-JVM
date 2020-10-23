//
// Created by zhukovasky on 2020/8/17.
//
#include "../../utils/StringUtils.h"
#include "Entry.h"
#include "../../utils/ZipUtils.h"
#include "../../utils/FileUtils.h"
namespace Runtime{
    namespace Heap{


        Entry *Entry::newEntry(std::string cpOption) {
            if(Utils::StringUtils::endsWith(cpOption,".jar")||Utils::StringUtils::endsWith(cpOption,".zip")){
                return new ZipEntry(cpOption);
            }

            if(Utils::StringUtils::endsWith(cpOption,"*")){
                return new WildcardEntry(cpOption);
            }

            return new DirEntry(cpOption);
        }



        Lang::ByteQueue *Entry::getByteQueue() const {
            return byteQueue;
        }

        void Entry::setByteQueue(Lang::ByteQueue *byteQueue) {
            Entry::byteQueue = byteQueue;
        }

        CompositeEntry::CompositeEntry() {}

        CompositeEntry::~CompositeEntry() {

        }

        Entry *CompositeEntry::readClass(std::string className) {
            return nullptr;
        }

        DirEntry::DirEntry(std::string path) {
            this->absDir=path;
        }

        DirEntry::~DirEntry() {

        }

        Entry *DirEntry::readClass(std::string className) {
            std::string content=Utils::FileUtils::readFileContent(this->absDir+"/"+className);
            if(content.empty()){
                return nullptr;
            }
            Lang::ByteQueue *byteQueue=new Lang::ByteQueue();
            byteQueue->pushStringChars(content);
            this->setByteQueue(byteQueue);
            return this;
        }

        WildcardEntry::WildcardEntry() {}
        WildcardEntry::WildcardEntry(std::string jreLibPath) {}

        Entry *WildcardEntry::readClass(std::string className) {
            return nullptr;
        }

        ZipEntry::ZipEntry() {}
        ZipEntry::ZipEntry(std::string path) {
            this->absDir=path;
        }

        Entry *ZipEntry::readClass(std::string className) {

            std::string content=ZipUtils::readZipFile(this->absDir,className);
            if(content.empty()){
                return nullptr;
            }
            Lang::ByteQueue *byteQueue=new Lang::ByteQueue();
            byteQueue->pushStringChars(content);
            this->setByteQueue(byteQueue);
            return this;
        }
    }
}