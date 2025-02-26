#pragma once
#ifndef __MZOOKEPPERUTIL__H__
#define __MZOOKEPPERUTIL__H__

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient
{
    public:
        ZkClient();
        ~ZkClient();
        void start();
        // 创建zk节点
        void create(const char* path,const char* data,int datalen,int state=0);
        // 获取节点数据
        std::string getData(const char* path);
    private:
        zhandle_t* m_zhandle;
};

#endif // !__MZOOKEPPERUTIL__H__