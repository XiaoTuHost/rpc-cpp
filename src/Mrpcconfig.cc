#include "Mrpcconfig.h"
#include <memory>


void Mrpcconfig::loadConfigFile(const char* config_file)
{
    // 独占智能指针管理FILE资源
    // 第二个模板参数位删除器 使用fclose关闭文件
    std::unique_ptr<FILE,decltype(&fclose)> pf(fopen(config_file,"r"),&fclose);
    if(pf == nullptr){
        exit(EXIT_FAILURE);
    }

    // 存储参数
    char buf[1024];
    // 使用pf.get()获取原始指针
    while(fgets(buf,1024,pf.get())!=nullptr){
        // 1.利用fgets获取行
        // 2.去掉前后空格
        std::string read_buf(buf);
        trim(read_buf);
        // 3.排除空行和注释
        if(read_buf[0]=='#' || read_buf.empty())    continue;
        // 4.分割KV
        int index = read_buf.find('=');
        if(index==-1) continue;
        // 4.1.获取KEY
        std::string key = read_buf.substr(0,index);
        trim(key);
        // 4.2.获取VALUE
        int end = read_buf.find('\n',index);
        std::string value = read_buf.substr(index+1,end-index-1);
        trim(value);
        
        // 5.存储KV
        config_map.insert({key,value});
    }
}

// 配置以KV形式存储在unordered_map中
std::string Mrpcconfig::load(const std::string key)
{
    auto it = config_map.find(key);
    if(it==config_map.end())
        return "";
    return it->second;
}

void Mrpcconfig::trim(std::string &read_buf)
{
    int index = read_buf.find_first_not_of(' ');
    if(index!=-1){
        read_buf = read_buf.substr(index,read_buf.size()-index);
    }
    index = read_buf.find_last_not_of(' ');
    if(index!=-1){
        read_buf = read_buf.substr(0,index+1);
    }
}