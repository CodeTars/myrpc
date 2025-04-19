#include "zookeeperutil.h"
#include <semaphore.h>
#include "logger.h"
#include "rpcapplication.h"

void watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
    // watcher回调函数
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            // watcherCtx是一个信号量指针
            sem_t* sem = static_cast<sem_t*>(watcherCtx);
            sem_post(sem);  // Signal that the connection is established
        }
    }
}

zkClient::zkClient() : zhandle(nullptr) {
    // Constructor implementation
}

zkClient::~zkClient() {
    // Destructor implementation
    if (zhandle) {
        zookeeper_close(zhandle);
    }
}

void zkClient::start() {
    std::string ip = RPCApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = RPCApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string host = ip + ":" + port;

    // 创建一个信号量
    sem_t sem;
    sem_init(&sem, 0, 0);  // Initialize the semaphore

    zhandle = zookeeper_init(host.c_str(), &watcher, 30000, nullptr, static_cast<void*>(&sem), 0);
    if (nullptr == zhandle) {
        // 连接失败
        logError("Failed to connect to zookeeper server: %s", host.c_str());
        exit(EXIT_FAILURE);
    }

    // 等待连接建立
    sem_wait(&sem);
    // 连接成功
    logInfo("Connected to zookeeper server: %s", host.c_str());
    // 关闭信号量
    sem_destroy(&sem);
}

int zkClient::createNode(const std::string& path, const std::string& data, int state) {
    int flag = zoo_exists(zhandle, path.c_str(), 0, nullptr);
    if (flag == ZOK) {
        // 节点已存在
        logInfo("Node already exists: %s", path.c_str());
        return 0;
    } else if (flag != ZNONODE) {
        // 其他错误
        logError("Failed to check zookeeper node existence: %s", path.c_str());
        return -1;
    }

    char path_buffer[128];
    // 创建zookeeper节点
    flag = zoo_create(zhandle, path.c_str(), data.c_str(), data.size(), &ZOO_OPEN_ACL_UNSAFE, state,
                      path_buffer, sizeof(path_buffer));
    if (flag == ZOK) {
        logInfo("Created zookeeper node: %s", path_buffer);
    } else {
        logError("Flag: %d, Failed to create zookeeper node: %s", flag, path_buffer);
        exit(EXIT_FAILURE);
    }
    return 0;
}

std::string zkClient::getNodeData(const std::string& path) {
    char buffer[64];
    int buffer_len = sizeof(buffer);
    int flag = zoo_get(zhandle, path.c_str(), 0, buffer, &buffer_len, nullptr);
    if (flag == ZOK) {
        logInfo("Get zookeeper node data: %s", buffer);
        return std::string(buffer, buffer_len);
    } else {
        logError("Failed to get zookeeper node data: %s", path.c_str());
        return "";
    }
}