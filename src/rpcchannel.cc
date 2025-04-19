#include "rpcchannel.h"
#include <arpa/inet.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "rpcapplication.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"

/**
 * 1. 序列化请求：头长度 + 请求头 + 请求参数
 * 2. 发送请求: 建立普通tcp连接后发送
 * 3. 接受响应: 读取响应数据后关闭连接
 * 4. 解析（反序列化）响应
 */
void MyRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                              google::protobuf::RpcController* controller,
                              const google::protobuf::Message* request,
                              google::protobuf::Message* response,
                              google::protobuf::Closure* done) {
    // 1. 序列化请求
    // 请求字符串：4字节header长度 + 请求头 + 参数
    std::string service_name = method->service()->name();
    std::string method_name = method->name();
    std::string args_str;
    if (!request->SerializeToString(&args_str)) {
        controller->SetFailed("Failed to serialize request");
        return;
    }

    // header: 服务名 + 方法名 + 参数长度
    myrpc::RequestHeader header;
    header.set_service_name(service_name);
    header.set_method_name(method_name);
    header.set_args_size(args_str.size());
    std::string header_str;
    if (!header.SerializeToString(&header_str)) {
        controller->SetFailed("Failed to serialize header");
        return;
    }

    int header_len = header_str.size();
    std::string send_buf;
    send_buf.insert(0, reinterpret_cast<char*>(&header_len), sizeof(int));
    send_buf += header_str;
    send_buf += args_str;

    // 获取配置中的ip和端口
    // std::string ip = RPCApplication::GetInstance().GetConfig().Load("rpcserverip");
    // std::string port = RPCApplication::GetInstance().GetConfig().Load("rpcserverport");

    // 从zookeeper注册中心获取ip和端口
    zkClient zk_client;
    zk_client.start();
    std::string host = zk_client.getNodeData("/" + service_name + "/" + method_name);
    if (host.empty()) {
        controller->SetFailed("Failed to get IP and port from zookeeper: /" + service_name + "/" +
                              method_name);
        return;
    }
    std::string::size_type pos = host.find(":");
    if (pos == std::string::npos) {
        controller->SetFailed("Invalid IP and port format: " + host);
        return;
    }
    std::string ip = host.substr(0, pos);
    std::string port = host.substr(pos + 1);

    // 发送请求
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::stoi(port));
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        controller->SetFailed("Failed to connect to server");
        close(sockfd);
        return;
    }
    if (send(sockfd, send_buf.c_str(), send_buf.size(), 0) < 0) {
        controller->SetFailed("Failed to send request");
        close(sockfd);
        return;
    }

    // 接受响应
    char recv_buf[4096];
    int recv_len = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
    if (recv_len < 0) {
        controller->SetFailed("Failed to receive response");
        close(sockfd);
        return;
    }
    // 解析响应
    if (response->ParseFromArray(recv_buf, recv_len)) {
        std::cout << "Response: \n" << response->DebugString() << std::endl;
    } else {
        controller->SetFailed("Failed to parse response");
        close(sockfd);
    }
    // 关闭连接
    close(sockfd);
}