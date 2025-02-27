#include "Mrpcapplication.h"
#include <stdlib.h>
#include <unistd.h>

Mrpcconfig  MrpcApplication::m_config;
std::mutex MrpcApplication::m_mutex;
MrpcApplication* MrpcApplication::m_application = nullptr;

void MrpcApplication::init(int argc,char** argv)
{
    // 参数错误/缺少参数
    auto err_usage = []{
        std::cout<<"format:command -i <configfile>"<<std::endl;
        exit(EXIT_FAILURE);
    };
    // 1.传入配置参数
    if(argc<2){
        err_usage();
    }
    // 2.解析参数
    int o;
    std::string configFile;
    const char* optString = "i:";
    while(-1!=(o=getopt(argc,argv,optString))){
        switch(o){
            // 正确解析
            case 'i':
                // 将-i 后面的参数赋值给configFile
                configFile=optarg;
                break;
            // 其它参数
            case '?':
                err_usage();
                break;
            // 出现了选项没有参数
            case ':':
                err_usage();
                break;
        } 
    }
    // 3.记载配置
    m_config.loadConfigFile(configFile.c_str());
}

// 获取实例
MrpcApplication& MrpcApplication::getInstance()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_application==nullptr){
        m_application = new MrpcApplication();
        atexit(deleteInstance);
    }
    return *m_application;
}

// 删除实例
void MrpcApplication::deleteInstance()
{
    if(m_application){
        delete m_application;
    }
}

// 获取配置
Mrpcconfig& MrpcApplication::getConfig()
{
    return m_config;
}