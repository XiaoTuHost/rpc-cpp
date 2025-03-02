#include <iostream>
#include <string>
#include "../user.pb.h"
#include "Mrpcapplication.h"
#include "Mrpcprovider.h"

class Userservice : public Muser::UserServiceRpc
{
    public:
        bool Login(std::string name,std::string pwd){
            std::cout<<"doing local service : login"<<std::endl;
            std::cout<<"name : "<<name<<" | pwd : "<<pwd<<std::endl;
            return true;
        }

    // 重写虚函数
    // 多态
        void Login(::google::protobuf::RpcController* controller
                    , const ::Muser::LoginRequest* request
                    , ::Muser::LoginResponse* response
                    , ::google::protobuf::Closure* done)
        {
            std::string name = request->name();
            std::string pwd = request->pwd();

            bool login_result = Login(name,pwd);

            Muser::ResultCode* code = response->mutable_result();
            code->set_errcode(0);
            code->set_errmsg("");
            response->set_success(login_result);

            done->Run();
        }
};

int main(int argc,char** argv){

    MrpcApplication::init(argc,argv);

    MrpcProvider provider;
    provider.notifyService(new Userservice());

    provider.run();

    return 0;
}