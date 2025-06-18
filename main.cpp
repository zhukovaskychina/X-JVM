#include "utils/PlatformCompat.h"
#include <iostream>
#include "utils/FileUtils.h"
#include "runtime/ThreadPool.h"
#include "runtime/ClassLoader.h"
#include "runtime/heap/JavaClass.h"
#include "interpret/Interpret.h"
#include <boost/program_options.hpp>



using namespace std;
using namespace boost;

using namespace boost::program_options;
std::mutex mtx;


using namespace std;

int main(int argc, char **argv)
{

    mtx.lock();


    try {
        options_description desc("zhukovasky的java   \n"
                                 "author: zhukovasky\n"
                                 "Usage: jvm [-options]  [args...]\n"
                                 " e.g jvm --help  \n"
                                 "     \n");
        desc.add_options()
                ("help", "帮助命令")
                ("classpath",value<string>(),
                        "jvm --classpath xxx; \n")
                ("xjre",value<string>(),"xjrehome")
                ("javaclass",value<string>(),"")
                ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 1;
        }
        std::string classpath="";
        std::string xjre="";
        std::string javaClassName="";
        if (vm.count("classpath")) {
            classpath=vm["classpath"].as<string>();
        }
        if(vm.count("javaclass")){
            javaClassName=vm["javaclass"].as<string>();
        }
        if(vm.count("xjre")){
            xjre=vm["xjre"].as<string>();
        }
        else{
            cout<< desc <<endl;
            exit(0);
        }
     //   cout<<classpath<<endl;
     //   cout<<javaClassName<<endl;
      //  exit(0);
//      Runtime::JavaRuntimeEnv javaRuntimeEnv;
//        javaRuntimeEnv.initRuntime();
        Runtime::ThreadPool threadPool;
        threadPool.start(1);
        std::future<void> result= threadPool.submit([=]() -> void{
         //   cout<<"hello"<<endl;
            Runtime::ClassPath *classPath=new Runtime::ClassPath();
            classPath->parseBootAndExtClassPath(xjre);
            classPath->parseUserClassPath(classpath);
            Runtime::ClassLoader *classLoader=new Runtime::ClassLoader(classPath);
            //
            if(!Utils::StringUtils::endsWith(javaClassName,".class")){
                cerr<<"java虚拟机加载文件出错"<<endl;
                exit(0);
            }

            std::string javaName=Utils::StringUtils::replaceAll(Utils::StringUtils::replaceAll(javaClassName, ".", "/"),"/class",".class");
            Runtime::JavaClass* javaClass=classLoader->loadClass(javaName);
            Runtime::Heap::Method *mainMethod=javaClass->getMainMethod();
            if(mainMethod== nullptr){
                cerr<<"java虚拟机找不到主方法"<<endl;
                exit(0);
            }
            Interpret::Interpret *interpret;
            interpret->execByteCode(mainMethod);
            //执行引擎执行java
        });
        result.share().get();
        result.get();

//        std::vector<std::thread> threadVector=threadPool.threads;

     //   std::vector<std::thread>::iterator threadIterator;
      //  for (threadIterator=threadVector.begin(); threadIterator!=threadVector.end() ; ++threadIterator) {
   //         threadIterator.operator*();
      //  }

        threadPool.workQueue;
        mtx.unlock();
    }catch (std::exception& e){
        std::cout<<e.what()<<std::endl;
    }



    return 0;
}