#include "rpcapplication.h"
#include <unistd.h>
#include <iostream>
#include "logger.h"

RpcConfig RPCApplication::config;

const RPCApplication& RPCApplication::GetInstance() {
    static RPCApplication instance;
    return instance;
}

void RPCApplication::Init(int argc, char** argv) {
    int opt;
    if (argc != 3 || (opt = getopt(argc, argv, "i:")) != 'i') {
        std::cerr << "Usage: " << argv[0] << " -i <config_file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string config_file(argv[2]);
    config.LoadConfigFile(config_file);

    std::cout << "=========================" << std::endl;
    std::cout << "rpcserverip:\t" << config.Load("rpcserverip") << std::endl;
    std::cout << "rpcserverport:\t" << config.Load("rpcserverport") << std::endl;
    std::cout << "zookeeperip:\t" << config.Load("zookeeperip") << std::endl;
    std::cout << "zookeeperport:\t" << config.Load("zookeeperport") << std::endl;
    std::cout << "=========================" << std::endl;

    // logInfo("rpcserverip: %s", config.Load("rpcserverip").c_str());
    // logInfo("rpcserverport: %s", config.Load("rpcserverport").c_str());
    // logInfo("zookeeperip: %s", config.Load("zookeeperip").c_str());
    // logInfo("zookeeperport: %s", config.Load("zookeeperport").c_str());
    // logInfo("RPCApplication initialized.");
}

const RpcConfig& RPCApplication::GetConfig() const {
    return config;
}