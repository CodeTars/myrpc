#include "rpcprovider.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/callback.h>
#include <functional>
#include "logger.h"
#include "rpcapplication.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"

void RPCProvider::NotifyService(google::protobuf::Service* service) {
    ServiceInfo service_info;
    service_info.service = service;

    // 获取服务描述符
    const google::protobuf::ServiceDescriptor* servDesc = service->GetDescriptor();
    // 输出服务名称
    logInfo("Registering service: %s", servDesc->full_name().c_str());

    int method_count = servDesc->method_count();
    for (int i = 0; i < method_count; ++i) {
        const google::protobuf::MethodDescriptor* methodDesc = servDesc->method(i);
        const std::string& method_name = methodDesc->name();
        service_info.method_map.insert({method_name, methodDesc});
        logInfo("Registering method: %s", methodDesc->full_name().c_str());
    }

    // 存储服务信息
    serviceInfo_map.insert({servDesc->name(), service_info});
}

void RPCProvider::Run() {
    // 获取配置文件中的ip和端口
    std::string ip = RPCApplication::GetInstance().GetConfig().Load("rpcserverip");
    std::string port = RPCApplication::GetInstance().GetConfig().Load("rpcserverport");
    // 创建一个监听地址
    muduo::net::InetAddress listenAddr(ip, std::stoi(port));
    // 创建一个TcpServer对象
    muduo::net::TcpServer server(&loop, listenAddr, "RPCProvider");

    // 绑定连接回调函数
    server.setConnectionCallback(
        std::bind(&RPCProvider::onConnection, this, std::placeholders::_1));

    // 绑定消息回调函数
    server.setMessageCallback(std::bind(&RPCProvider::onMessage, this, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3));

    // 设置线程数
    server.setThreadNum(4);

    zkClient zk_client;
    zk_client.start();
    for (const auto& sit : serviceInfo_map) {
        std::string service_name = sit.first;
        std::string service_path = "/" + service_name;
        zk_client.createNode(service_path, "", 0);
        for (const auto& mit : sit.second.method_map) {
            std::string method_name = mit.first;
            std::string method_path = service_path + "/" + method_name;
            std::string data = ip + ":" + port;
            // 创建临时节点
            zk_client.createNode(method_path, data, ZOO_EPHEMERAL);
        }
    }

    // 启动服务
    server.start();
    // 进入事件循环
    loop.loop();
    return;
}

// 连接建立时的回调函数
void RPCProvider::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        conn->shutdown();
    }
}

void sendResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    // 序列化响应
    std::string response_str;
    if (response->SerializeToString(&response_str)) {
        conn->send(response_str);
    } else {
        logError("Failed to serialize response.");
    }
    delete response;
    conn->shutdown();
}

// 接收消息时的回调函数
void RPCProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,
                            muduo::net::Buffer* buf,
                            muduo::Timestamp time) {
    // 接受数据
    std::string recv_buf = buf->retrieveAllAsString();

    /**
     * 返序列化：4字节长度 + request_header_str + args_str
     * request_header_str = service_name + method_name + args_size
     */
    int header_len = 0;
    memcpy(&header_len, recv_buf.c_str(), sizeof(int));

    // 反序列化请求头
    std::string request_header_str = recv_buf.substr(sizeof(int), header_len);
    myrpc::RequestHeader request_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size = 0;
    // 解析请求头
    if (request_header.ParseFromString(request_header_str)) {
        service_name = request_header.service_name();
        method_name = request_header.method_name();
        args_size = request_header.args_size();
    } else {
        logError("Failed to parse request header: %s", request_header_str.c_str());
        return;
    }
    std::string args_str = recv_buf.substr(sizeof(int) + header_len, args_size);

    // 打印调试信息
    std::cout << "=====================================" << std::endl;
    std::cout << "header_size: " << header_len << std::endl;
    std::cout << "header_str: " << request_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "=====================================" << std::endl;
    // 打印日志
    logInfo("Received request: service_name=%s, method_name=%s, args_size=%u", service_name.c_str(),
            method_name.c_str(), args_size);
    // 打印接收数据
    logInfo("Received data: %s", recv_buf.c_str());
    // 打印请求头
    logInfo("Request header: %s", request_header_str.c_str());
    // 打印参数
    logInfo("Request args: %s", args_str.c_str());

    // 确认服务名正确
    if (!serviceInfo_map.count(service_name)) {
        logError("Service not found: %s", service_name.c_str());
        return;
    }
    // 获取服务信息
    ServiceInfo service_info = serviceInfo_map[service_name];
    // 确认方法名正确
    if (service_info.method_map.count(method_name) == 0) {
        logError("Method not found: %s", method_name.c_str());
        return;
    }
    // 获取方法描述符
    const google::protobuf::MethodDescriptor* method = service_info.method_map[method_name];

    // 反序列化参数
    google::protobuf::Message* request = service_info.service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        logError("Failed to parse request args. Content: %s", args_str.c_str());
        delete request;
        return;
    }

    // 调用方法
    google::protobuf::Message* response = service_info.service->GetResponsePrototype(method).New();
    google::protobuf::Closure* done =
        google::protobuf::NewCallback<const muduo::net::TcpConnectionPtr&,
                                      google::protobuf::Message*>(sendResponse, conn, response);
    service_info.service->CallMethod(method, nullptr, request, response, done);
}
