#include <iostream>
#include <boost/program_options.hpp>

#include "runtime/ClassLoader.h"
#include "runtime/heap/JavaClass.h"
#include "runtime/heap/JavaHeap.h"
#include "interpret/Interpret.h"
#include "utils/StringUtils.h"

using boost::program_options::options_description;
using boost::program_options::store;
using boost::program_options::parse_command_line;
using boost::program_options::variables_map;
using boost::program_options::notify;
using boost::program_options::value;

int main(int argc, char** argv)
{
    std::cout << "=== X-JVM Starting ===" << std::endl;

    try {
        options_description desc(
            "zhukovasky的java\n"
            "author: zhukovasky\n"
            "Usage: jvm [-options] [args...]\n"
            " e.g jvm --help\n");
        desc.add_options()
            ("help", "帮助命令")
            ("classpath", value<std::string>(), "类路径，例如 jvm --classpath .")
            ("xjre", value<std::string>(), "JRE 根目录")
            ("javaclass", value<std::string>(), "主类 .class 文件名");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        std::string classpath;
        std::string xjre;
        std::string javaClassName;
        if (vm.count("classpath")) {
            classpath = vm["classpath"].as<std::string>();
        }
        if (vm.count("javaclass")) {
            javaClassName = vm["javaclass"].as<std::string>();
        }
        if (vm.count("xjre")) {
            xjre = vm["xjre"].as<std::string>();
        } else {
            std::cout << desc << std::endl;
            return 0;
        }

        auto* classPath = new Runtime::ClassPath();
        classPath->parseBootAndExtClassPath(xjre);
        classPath->parseUserClassPath(classpath);
        auto* classLoader = new Runtime::ClassLoader(classPath);

        if (!Utils::StringUtils::endsWith(javaClassName, ".class")) {
            std::cerr << "java虚拟机加载文件出错" << std::endl;
            delete classLoader;
            delete classPath;
            return 1;
        }

        std::string javaName = Utils::StringUtils::replaceAll(
            Utils::StringUtils::replaceAll(javaClassName, ".", "/"), "/class", ".class");
        Runtime::JavaClass* javaClass = classLoader->loadClass(javaName);
        Runtime::Heap::Method* mainMethod = javaClass->getMainMethod();
        if (mainMethod == nullptr) {
            std::cerr << "java虚拟机找不到主方法" << std::endl;
            delete classLoader;
            delete classPath;
            return 1;
        }

        Runtime::JavaHeap javaHeap;
        Interpret::Interpret interpret;
        interpret.execByteCode(mainMethod, &javaHeap, classLoader);

        delete classLoader;
        delete classPath;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
