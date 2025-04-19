#pragma once
#include "rpcconfig.h"

class RPCApplication {
public:
    // 获取单例对象
    static const RPCApplication &GetInstance();
    // 初始化
    static void Init(int argc, char **argv);
    // 获取配置
    const RpcConfig &GetConfig() const;

private:
    RPCApplication() = default;
    RPCApplication(const RPCApplication &) = delete;
    RPCApplication(RPCApplication &&) = delete;
    RPCApplication &operator=(const RPCApplication &) = delete;
    RPCApplication &operator=(RPCApplication &&) = delete;
    ~RPCApplication() = default;

    static RpcConfig config;
};