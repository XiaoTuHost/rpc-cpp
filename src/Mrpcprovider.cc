#include "Mrpcprovider.h"
#include "Mrpcapplication.h"
#include "MrpcHeader.pb.h"
#include "MrpcLogger.h"
#include <iostream>

// 注册服务对象和rpc方法
// 保存到成员变量
void MrpcProvider::notifyService(google::protobuf::Service* service){
    // 1.从service获取服务方法列表
    ServiceInfo service_info;
    const google::protobuf::ServiceDescriptor* psd = service->GetDescriptor();    
    
    std::string service_name = psd->name();
    std::cout<<"service_name : "<<service_name<<std::endl;
    int method_count = psd->method_count();
    // 2.存储方法名和方法列表
    for(int i=0;i<method_count;++i){
        const google::protobuf::MethodDescriptor* pmd = psd->method(i);
        std::string method_name = pmd->name();
        std::cout<<"method_name : "<<method_name<<std::endl;
        service_info.method_map.emplace(method_name,pmd);
    }
    // 3.存储服务名和服务方法等信息
    service_info.service = service;
    service_map.emplace(service_name,service_info);
}

// 启动服务节点，开启rpc服务
void MrpcProvider::run(){
    // 1.获取rpcserver信息
    std::string ip = MrpcApplication::getInstance().getConfig().load("rpc_server_ip");
    uint16_t port = atoi(MrpcApplication::getInstance().getConfig().load("rpc_server_port").c_str());
    // 2.创建tcpServer对象
    muduo::net::InetAddress address(ip,port);
    std::shared_ptr<muduo::net::TcpServer> server = 
                std::make_shared<muduo::net::TcpServer>(&event_loop,address,"MrpcProvider");
    // 3.绑定连接回调和消息回调
    server->setConnectionCallback(std::bind(&MrpcProvider::onConnection,this,std::placeholders::_1));
    server->setMessageCallback(std::bind(&MrpcProvider::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    // 4.设置工作线程参数
    server->setThreadNum(3);
    // 5.利用zk将节点注册到注册中心
    ZkClient zkclient;
    zkclient.Start();
    for(const auto &sp:service_map){
        std::string service_path = sp.first;
        zkclient.Create(service_path.c_str(),nullptr,0);
        for(const auto& mp : sp.second.method_map){
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);
            zkclient.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);  
        }
    }
    // 6.启动网络服务
    // 工作线程初始化和事件监听
    std::cout<<"rpc server start at ip : "<<ip<<" | port : "<<port<<std::endl;
    server->start();
    event_loop.loop();
}

void MrpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn){
    if(!conn->connected()){
        conn->shutdown();
    }
}

// 重点->消息回调
void MrpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp receive_time)
{
    std::cout<<"call OnMessage()"<<std::endl;

    // 1.接收rpc请求
    std::string recv_buf = buffer->retrieveAllAsString();
    // 2.protobuf反序列化
    google::protobuf::io::ArrayInputStream raw_input(recv_buf.data(),recv_buf.size());
    google::protobuf::io::CodedInputStream coded_input(&raw_input);
    // 3.解析参数
    uint32_t header_size{};
    coded_input.ReadVarint32(&header_size);

    std::string rpc_header_str;
    Mrpc::RpcHeader mrpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size{};

    google::protobuf::io::CodedInputStream::Limit msg_limit=coded_input.PushLimit(header_size);
    coded_input.ReadString(&rpc_header_str,header_size);
    
    coded_input.PopLimit(msg_limit);
    if(mrpcHeader.ParseFromString(rpc_header_str)){
        service_name = mrpcHeader.service_name();
        method_name = mrpcHeader.method_name();
        args_size = mrpcHeader.args_size();
    }
    else{
        MrpcLogger::ERROR("MrpcHeader parse error");
        return;
    }
    // 获取了参数大小，开始读取方法参数
    std::string args_str;
    bool read_args_success = coded_input.ReadString(&args_str,args_size);
    if(!read_args_success){
        MrpcLogger::ERROR("read args error");
        return;
    }
    // 4.获取服务对象和服务方法
    auto it = service_map.find(service_name);
    if(it == service_map.end()){
        std::cout<<service_name<<" is not exit!"<<std::endl;
        return;
    }
    auto mit = it->second.method_map.find(method_name);
    if(mit == it->second.method_map.end()){
        std::cout<<method_name<<" is not exit"<<std::endl;
        return;
    }
    google::protobuf::Service * service = it->second.service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    // 5.生成rpc调用request和response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        std::cout<<service_name<<"."<<method_name<<"parse error!"<<std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 6.设置rpc调用回调
    google::protobuf::Closure* done = google::protobuf::NewCallback<MrpcProvider,
                                                        const muduo::net::TcpConnectionPtr&,
                                                        google::protobuf::Message*>(
                                                            this,
                                                            &MrpcProvider::sendRpcResponse,
                                                            conn,
                                                            response
                                                        );
    service->CallMethod(method,nullptr,request,response,done);                                                    
}

void MrpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str)){
        conn->send(response_str);
    }
    else{
        std::cout<<"serialize error!"<<std::endl;
    }
    // 可模拟短连接
}

MrpcProvider::~MrpcProvider()
{
    std::cout<<"call ~MrpcProvider"<<std::endl;
    event_loop.quit();
}