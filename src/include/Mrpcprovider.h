#pragma once
#ifndef __MRPCPROVIDER__H__
#define __MRPCPROVIDER__H__

#include "google/protobuf/service.h"
#include "zookeeperutil.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <functional>
#include <string>
#include <unordered_map>

// rpc核心服务实现
class MrpcProvider
{
    public:
        void notifyService(google::protobuf::Service* service);
        ~MrpcProvider();
        void run(); 
    private:
        muduo::net::EventLoop event_loop;
        struct ServiceInfo 
        {
            google::protobuf::Service* service;
            std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> method_map;
        };
        std::unordered_map<std::string,ServiceInfo>service_map;

        void onConnection(const muduo::net::TcpConnectionPtr& conn);
        void onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp receive_time);
        void sendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response);
};

#endif // !__MRPCPROVIDER__H__