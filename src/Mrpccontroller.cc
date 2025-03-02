#include "Mrpccontroller.h"
Mrpccontroller::Mrpccontroller()
    : m_failed(false)
    , m_errText("")
{
}

void Mrpccontroller::Reset(){
    m_failed = false;
    m_errText = "";
}

bool Mrpccontroller::Failed()const 
{
    return m_failed;
}

std::string Mrpccontroller::ErrorText() const
{
    return m_errText;
}

void Mrpccontroller::SetFailed(const std::string& reason){
    m_failed=true;
    m_errText = reason;
}

// TODO

void Mrpccontroller::StartCancel()
{

}

bool Mrpccontroller::IsCanceled() const
{

}

void Mrpccontroller::NotifyOnCancel(google::protobuf::Closure* callback)
{

}