//
// Created by zhukovasky on 2020/8/27.
//

#ifndef JVM_COMMON_H
#define JVM_COMMON_H


template <class T>
int GetArrayLength(T &array){
    int size= sizeof(array)/sizeof(array[0]);
    return size;
};


#endif //JVM_COMMON_H
