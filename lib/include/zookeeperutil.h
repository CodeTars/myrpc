#pragma once
#include <zookeeper/zookeeper.h>
#include <string>

class zkClient {
public:
    zkClient();
    ~zkClient();
    // 连接zookeeper服务器
    void start();
    // 创建zookeeper节点
    int createNode(const std::string& path, const std::string& data, int state = 0);
    // 获取zookeeper节点数据
    std::string getNodeData(const std::string& path);

private:
    zhandle_t* zhandle;
};
