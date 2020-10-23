//
// Created by 郑邦国 on 2018/12/24.
//

#ifndef JVM_ZIPUTILS_H
#define JVM_ZIPUTILS_H

#include <zlib.h>
#include "../libs/unzip/unzip.h"
#include "iostream"
#include "vector"
using namespace std;
class ZipUtils {
public:
    static std::string readZipFile(string zipFile, string fileInZip);
    static void getZipList(string zipFile, std::vector<string> *_vector);


};


#endif //JVM_ZIPUTILS_H
