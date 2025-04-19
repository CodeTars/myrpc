#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>

// 用于RPC客户端，若在服务端使用，可能会导致未知错误
class MyRpcController : public google::protobuf::RpcController {
public:
    MyRpcController();
    // 恢复到初始状态，以便可以在新调用中重用
    void Reset() override;

    // 是否失败
    bool Failed() const override;

    // 设置错误信息
    void SetFailed(const std::string &reason) override;

    // 获取错误信息
    std::string ErrorText() const override;

    // 目前未实现
    void StartCancel() override;
    bool IsCanceled() const override;
    void NotifyOnCancel(google::protobuf::Closure *callback) override;

private:
    bool failed_ = false;    // 是否失败
    std::string error_text_; // 错误信息
};