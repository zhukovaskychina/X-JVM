//
// Created by zhukovasky on 2020/8/4.
//

#ifndef JAVAP_STRINGUTILS_H
#define JAVAP_STRINGUTILS_H

#include "PlatformCompat.h"
#include <string>
#include <algorithm>
#include <vector>
#include <memory.h>
#include <sstream>
#include <codecvt>
#include <iostream>
#include <locale>

namespace Utils{
    using namespace std;
    class StringUtils {
    public:
        static u16string convertStringToWS(std::string str){
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t > converter;
            return converter.from_bytes(str);
        }

        static string convertWStringToString(u16string str16){
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t > converter;
            return converter.to_bytes(str16);
        }
/*
 函数说明：对字符串中所有指定的子串进行替换
 参数：
string resource_str            //源字符串
string sub_str                //被替换子串
string new_str                //替换子串
返回值: string
 */
        static string replaceAll(string resource_str, string sub_str, string new_str)
        {
            string::size_type pos = 0;
            while((pos = resource_str.find(sub_str)) != string::npos)   //替换所有指定子串
            {
                resource_str.replace(pos, sub_str.length(), new_str);
            }
            return resource_str;
        }
        static std::string trimLeft(const std::string& str, const std::string& token = " ")
        {
            std::string t = str;
            t.erase(0, t.find_first_not_of(token));
            return t;
        }

        static std::string trimRight(const std::string& str, const std::string& token = " ")
        {
            std::string t = str;
            t.erase(t.find_last_not_of(token) + 1);
            return t;
        }
        /*****
         *
         *
         * **/
        static std::string trim(const std::string& str, const std::string& token = " ")
        {
            std::string t = str;
            t.erase(0, t.find_first_not_of(token));
            t.erase(t.find_last_not_of(token) + 1);
            return t;
        }

        static std::string toLower(const std::string& str)
        {
            std::string t = str;
            std::transform(t.begin(), t.end(), t.begin(), ::tolower);
            return t;
        }

        static std::string toUpper(const std::string& str)
        {
            std::string t = str;
            std::transform(t.begin(), t.end(), t.begin(), ::toupper);
            return t;
        }

        static bool startsWith(const std::string& str, const std::string& substr)
        {
            if (str.length() < substr.length()) {
                return false;
            }
            return str.find(substr) == 0;
        }

        static bool equalsIgnoreCase(const std::string& str1, const std::string& str2)
        {
            return toLower(str1) == toLower(str2);
        }

        // Use cross-platform split implementation
        static std::vector<std::string> split(const std::string& str, const std::string& delimiter)
        {
            return PlatformCompat::split(str, delimiter);
        }


        static bool endsWith(const std::string& str, const std::string& substr)
        {
            if (str.length() < substr.length()) {
                return false;
            }
            return str.rfind(substr) == (str.length() - substr.length());
        }
        static std::string StringToHex(const std::string& data)
        {
            const std::string hex = "0123456789ABCDEF";
            std::stringstream ss;

            for (std::string::size_type i = 0; i < data.size(); ++i)
                ss << hex[(unsigned char)data[i] >> 4] << hex[(unsigned char)data[i] & 0xf];

            return ss.str();
        }

        static std::string HexToStr(const std::string& str)
        {
            std::string result;
            for (size_t i = 0; i < str.length(); i += 2)
            {
                std::string byte = str.substr(i, 2);
                char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
                result.push_back(chr);
            }
            return result;
        }
        static inline std::string LastIndexOf(const std::string& str,std::string index)
        {


        }

    };
}


#endif //JAVAP_STRINGUTILS_H


