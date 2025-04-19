#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <unordered_map>

class RPCProvider {
  public:
    void NotifyService(google::protobuf::Service *service);
    void Run();

  private:
    // 事件循环
    muduo::net::EventLoop loop;

    struct ServiceInfo {
        // RPC服务的名称
        google::protobuf::Service *service;
        // RPC方法到方法描述符的映射
        std::unordered_map<std::string,
                           const google::protobuf::MethodDescriptor *>
            method_map;
    };
    std::unordered_map<std::string, ServiceInfo> serviceInfo_map;

    // 连接回调函数
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    // 消息回调函数
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf, muduo::Timestamp time);

    // void sendResponse(google::protobuf::Message *response);
};