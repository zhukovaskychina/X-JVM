//
// 跨平台小工具：路径分隔、读文件、fmod 等与编译器/OS 相关的封装。
//

#ifndef XJVM_PLATFORM_COMPAT_H
#define XJVM_PLATFORM_COMPAT_H

#include <string>
#include <vector>

namespace PlatformCompat {

std::vector<std::string> split(const std::string& str, const std::string& delimiter);

std::string readFileContent(const std::string& filePath);

float safeFmod(float a, float b);

double safeFmod(double a, double b);

}  // namespace PlatformCompat

#endif
