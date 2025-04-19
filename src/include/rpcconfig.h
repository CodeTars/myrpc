#pragma once
#include <unordered_map>

class RpcConfig {
  public:
    // 取出配置项
    std::string Load(const std::string &key) const;
    // 导入配置文件
    void LoadConfigFile(const std::string &config_file);

  private:
    std::unordered_map<std::string, std::string> config_map;
    void Trim(std::string &str);
};