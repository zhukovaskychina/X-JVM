#include "PlatformCompat.h"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

namespace PlatformCompat {

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    if (delimiter.empty()) {
        if (!str.empty()) {
            result.push_back(str);
        }
        return result;
    }
    std::string::size_type start = 0;
    std::string::size_type pos = 0;
    while ((pos = str.find(delimiter, start)) != std::string::npos) {
        result.push_back(str.substr(start, pos - start));
        start = pos + delimiter.size();
    }
    result.push_back(str.substr(start));
    return result;
}

std::string readFileContent(const std::string& filePath) {
    std::ifstream in(filePath.c_str(), std::ios::binary);
    if (!in) {
        return "";
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

float safeFmod(float a, float b) {
    if (b == 0.0f) {
        return std::numeric_limits<float>::quiet_NaN();
    }
    return std::fmod(a, b);
}

double safeFmod(double a, double b) {
    if (b == 0.0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return std::fmod(a, b);
}

}  // namespace PlatformCompat
