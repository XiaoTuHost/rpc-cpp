#include "Mrpccontroller.h"
Mrpccontroller::Mrpccontroller()
    : m_failed(false)
    , m_errText("")
{
}

void Mrpccontroller::reset(){
    m_failed = false;
    m_errText = "";
}

bool Mrpccontroller::failed()const 
{
    return m_failed;
}

std::string Mrpccontroller::errText() const
{
    return m_errText;
}

void Mrpccontroller::setFailed(const std::string& reason){
    m_failed=true;
    m_errText = reason;
}