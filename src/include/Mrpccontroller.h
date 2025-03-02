#pragma once
#ifndef __MRPCCONTROLLER__H__
#define __MRPCCONTROLLER__H__

#include <google/protobuf/service.h>
#include <string>

// 跟踪RPC调用状态
// 继承rpcController 实现虚基类
class Mrpccontroller : public google::protobuf::RpcController
{
    public:
        Mrpccontroller();
        // 需要重写纯虚函数
        void Reset();
        bool Failed() const;
        std::string ErrorText() const;
        void SetFailed(const std::string& reason);

        // TODO
        void StartCancel();
        bool IsCanceled() const;
        void NotifyOnCancel(google::protobuf::Closure* callback);
    private:
        bool m_failed;
        std::string m_errText;
};

#endif // !__MRPCCONTROLLER__H__