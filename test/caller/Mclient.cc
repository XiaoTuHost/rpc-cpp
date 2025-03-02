#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include "../user.pb.h"
#include "Mrpcapplication.h"
#include "Mrpccontroller.h"
#include "MrpcLogger.h"

void send_request(int thread_id,std::atomic<int>& success_count,std::atomic<int>& failed_count)
{
    Muser::UserServiceRpc_Stub stub(new MrpcChannel(false));

    Muser::LoginRequest request;
    request.set_name("awei");
    request.set_pwd("123456");

    Muser::LoginResponse response;
    Mrpccontroller controller;
    stub.Login(&controller,&request,&response,nullptr);
    if(controller.Failed()){
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else{
        if(0==response.result().errcode()){
            std::cout<<"rpc login response success:"<<response.success()<<std::endl;
            success_count++;
        }
        else{
            std::cout<<"rpc login response error"<<response.result().errmsg()<<std::endl;
            failed_count++;
        }
    }
}

int main(int argc,char**argv)
{
    MrpcApplication::init(argc,argv);
    MrpcLogger logger("MyRpc");
    const int thread_count =10;
    const int requests_per_thread = 1;

    std::vector<std::thread> threads;
    std::atomic<int> success_count(0);
    std::atomic<int> failed_count(0);

    auto start_time = std::chrono::high_resolution_clock::now();
    for(int i = 0;i<thread_count;++i){
        threads.emplace_back([argc,argv,i,&success_count,&failed_count,requests_per_thread]()
            {
                for(int j=0;j<requests_per_thread;++j){
                    send_request(i,success_count,failed_count);
                }      
            }
        );
    }

    for(auto &t:threads){
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    LOG(INFO)<<"Total requests : "<<thread_count*requests_per_thread;
    LOG(INFO)<<"Success count : "<<success_count;
    LOG(INFO)<<"Failed count : "<<failed_count;
    LOG(INFO)<<"Elapsed time : "<<elapsed.count()<<" seconds";
    LOG(INFO)<<"QPS : "<<(thread_count*requests_per_thread)/elapsed.count();

    return 0;
}