#pragma once
#ifndef __MRPCCHANNEL__H__
#define __MRPCCHANNEL__H__

#include <google/protobuf/service.h>
#include <zookeeperutil.h>

class MrpcChannel : public google::protobuf::RpcChannel
{
    public:
        MrpcChannel(bool connectNow);
        virtual ~MrpcChannel()
        {
        }
        /*
        virtual void CallMethod(
            const MethodDescriptor* method,      // 被调用的方法描述符（自动生成）
            RpcController* controller,           // 控制调用的上下文（如错误处理）
            const Message* request,              // 请求消息（序列化后发送）
            Message* response,                   // 响应消息（反序列化后填充）
            Closure* done                        // 异步回调（可选）
        ) = 0;
         */
        void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                        ::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Message* request,
                        ::google::protobuf::Message* response,
                        ::google::protobuf::Closure* done
                    ) override;
        
    private:
        int m_clientfd;
        std::string service_name;
        std::string m_ip;
        uint16_t m_port;
        std::string method_name;
        int m_index;
        
        bool newConnect(const char* ip,uint16_t port);
        std::string QueryServiceHost(ZkClient* zkClient,
            std::string service_name,
            std::string method_name,
            int &idx);
};

#endif // !__MRPCCHANNEL__H__