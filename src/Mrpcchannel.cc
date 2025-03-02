#include "Mrpcchannel.h"
#include "MrpcLogger.h"
#include "Mrpcapplication.h"
#include "Mrpccontroller.h"
#include "MrpcHeader.pb.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <memory>

void MrpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                                ::google::protobuf::RpcController* controller,
                                const ::google::protobuf::Message* request,
                                ::google::protobuf::Message* response,
                                ::google::protobuf::Closure* done )
{
        // 1.获取服务名和方法名
        if(-1 == m_clientfd){
            const google::protobuf::ServiceDescriptor* sd  = method->service();
            service_name = sd->name();
            method_name = method->name();
            
            // 在zk上查询信息
            ZkClient zkClient;
            zkClient.Start();
            std::string host_data = QueryServiceHost(&zkClient,service_name,method_name,m_index);
            m_ip = host_data.substr(m_index);
            std::cout<<"ip : "<<m_ip<<std::endl;
            m_port = atoi(host_data.substr(m_index+1,host_data.size()-m_index).c_str());
            std::cout<<"port : "<<m_port<<std::endl;
            auto rt = newConnect(m_ip.c_str(),m_port);
            if(!rt){
                LOG(ERROR)<<"connect server error";
            }else{
                LOG(INFO)<<"connect server success";
            }
        }
        // 2.获取参数大小
        uint32_t args_size{};
        std::string args_str;
        if(request->SerializeToString(&args_str)){
            args_size = args_str.size();
        }else{
            controller->SetFailed("serialized request failed");
            return;
        }
        // 3.构造header
        Mrpc::RpcHeader mrpcheader;
        mrpcheader.set_service_name(service_name);
        mrpcheader.set_method_name(method_name);
        mrpcheader.set_args_size(args_size);

        uint32_t header_size{};
        std::string rpc_header_str;
        if(mrpcheader.SerializeToString(&rpc_header_str)){
            header_size = rpc_header_str.size();
        }else{
            controller->SetFailed("serialized rpc header error!");
            return;
        }
        // 4.发送rpc请求
        std::string send_rpc_str;
        {
            google::protobuf::io::StringOutputStream string_output(&send_rpc_str);
            google::protobuf::io::CodedOutputStream coded_output(&string_output);
            coded_output.WriteVarint32(static_cast<uint32_t>(header_size));
            coded_output.WriteString(rpc_header_str);
        }
        send_rpc_str+=args_size;
        // 5.接收rpc响应
        if(-1==send(m_clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)){
            close(m_clientfd);
            char errtext[512]{};
            std::cout<<"send error : "<<strerror_r(errno,errtext,sizeof(errtext))<<std::endl;
            controller->SetFailed("send error");
            return;
        }
        // 6.反序列化接收
        char recv_buf[1024] = {0};
        int recv_size = 0;
        if(-1 == (recv_size = recv(m_clientfd,recv_buf,1024,0))){
            close(m_clientfd);
            char errtext[512]{};
            std::cout<<"recv error : "<<strerror_r(errno,errtext,sizeof(errtext))<<std::endl;
            controller->SetFailed("recv error");
            return;
        }
        if(response->ParseFromArray(recv_buf,recv_size)){
            close(m_clientfd);
            char errtext[512]{};
            std::cout<<"parse error : "<<strerror_r(errno,errtext,sizeof(errtext))<<std::endl;
            controller->SetFailed("parse error");
            return;
        }
        close(m_clientfd);
}

// 连接rpc服务
// socket编程应用
bool MrpcChannel::newConnect(const char* ip,uint16_t port)
{
    // 1.准备socket
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == clientfd){
        char errtext[512] = {0};
        std::cout<<"socket error!"<<strerror_r(errno,errtext,sizeof(errtext))<<std::endl;
        LOG(ERROR)<<"socket error : "<<errtext;
        return false;
    }
    // 2.地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    // 3.连接目标主机
    if(-1 == connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        close(clientfd);
        char errtext[512] = {0};
        std::cout << "connect"<<strerror_r(errno,errtext,sizeof(errtext))<<std::endl;
        LOG(ERROR)<<"connect server error"<<errtext;
        return false;
    }
    m_clientfd = clientfd;
    return true;
}

std::string MrpcChannel::QueryServiceHost(
    ZkClient *zkClient,
    std::string service_name,std::string method_name,int &idx)
{

}

MrpcChannel::MrpcChannel(bool connectNow)
    : m_clientfd(-1)
    , m_index(0)
{
    if(!connectNow){
        return;
    }
    auto rt = newConnect(m_ip.c_str(),m_port);
    int count =3;

    // 失败重试连接
    while(!rt && count--){
        rt = newConnect(m_ip.c_str(),m_port);
    }
}   

MrpcChannel::~MrpcChannel()
{

}