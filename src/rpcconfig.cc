#include "rpcconfig.h"
#include <fstream>
#include <iostream>

std::string RpcConfig::Load(const std::string &key) const {
    auto it = config_map.find(key);
    if (it != config_map.end()) {
        return it->second;
    }
    return "";
}

void RpcConfig::LoadConfigFile(const std::string &config_file) {
    // 读取配置文件的逻辑
    // 假设配置文件是一个简单的key=value格式
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << config_file << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        Trim(line);
        if (line[0] == '#' || line.empty())
            continue;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            Trim(key);
            Trim(value);
            if (config_map.count(key)) {
                std::cerr << "Duplicate key found in config file: " << key << std::endl;
            } else {
                config_map.insert({key, value});
            }
        } else {
            std::cerr << "Invalid line in config file: " << line << std::endl;
        }
    }
}

void RpcConfig::Trim(std::string &str) {
    // 去除字符串两端的空格
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
        str = str.substr(start, end - start + 1);
    } else {
        str.clear();
    }
}