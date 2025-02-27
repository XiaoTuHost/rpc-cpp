#pragma once
#ifndef __MRPCCONFIG__H__
#define __MRPCCONFIG__H__

#include <unordered_map>
#include <string>

class Mrpcconfig
{
    public:
        // 从配置文件获取信息
        void loadConfigFile(const char* config_file);
        std::string load(const std::string key);
    private:
        // 存储配置信息
        std::unordered_map<std::string,std::string> config_map;
        // 处理获取的配置信息的字符串 删除前后空格
        void trim(std::string &read_buf);
};

#endif // !__MRPCCONFIG__H__