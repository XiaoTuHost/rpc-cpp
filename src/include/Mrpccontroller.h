#pragma once
#ifndef __MRPCCONTROLLER__H__
#define __MRPCCONTROLLER__H__

#include <google/protobuf/service.h>
#include <string>

// 跟踪RPC调用状态
class Mrpccontroller
{
    public:
        Mrpccontroller();
        void reset();
        bool failed() const;
        std::string errText() const;
        void setFailed(const std::string& reason);
    private:
        bool m_failed;
        std::string m_errText;
};

#endif // !__MRPCCONTROLLER__H__