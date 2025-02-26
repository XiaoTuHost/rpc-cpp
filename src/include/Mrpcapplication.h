#pragma once
#ifndef __MRPCAPPLICATION__H__
#define __MRPCAPPLICATION__H__

#include "Mrpcconfig.h"
#include "Mrpcchannel.h"
#include "Mrpccontroller.h"
#include <mutex>

// rpc基础类
// 负责初始化操作 获取配置信息
// 单例
class MrpcApplication
{
    public:
        static void init(int argc,char** argv);
        static MrpcApplication& getInstance();
        static void deleteInstance();
        static Mrpcconfig& getConfig();
    private:
        static Mrpcconfig m_config;
        static MrpcApplication* m_application;
        static std::mutex m_mutex;
        MrpcApplication(){}
        ~MrpcApplication(){}
        // 删除拷贝构造函数、移动构造函数
        MrpcApplication(const MrpcApplication&)=delete;
        MrpcApplication(MrpcApplication&&)=delete;
};

#endif // !__MRPCAPPLICATION__H__