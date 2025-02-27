#pragma once
#ifndef __MRPCLOGGER__H__
#define __MRPCLOGGER__H__

#include <glog/logging.h>
#include <string>

// RAII
class MrpcLogger
{
    public: 
        explicit MrpcLogger(const char* argv0)
        {
            google::InitGoogleLogging(argv0);
            FLAGS_colorlogtostderr=true;
            FLAGS_logtostderr=true;
        }

        ~MrpcLogger(){
            google::ShutdownGoogleLogging();
        }

        static void info(const std::string& message){
            LOG(INFO)<<message;
        }

        static void warning(const std::string& message){
            LOG(WARNING)<<message;
        }   

        static void error(const std::string& message){
            LOG(ERROR)<<message;
        }

        static void fatal(const std::string& message){
            LOG(FATAL)<<message;
        }

    private:
};

#endif // !__MRPCLOGGER__H__