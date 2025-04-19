#include "rpccontroller.h"

MyRpcController::MyRpcController() : failed_(false), error_text_("") {}

void MyRpcController::Reset() {
    failed_ = false;
    error_text_.clear();
}

bool MyRpcController::Failed() const { return failed_; }

void MyRpcController::SetFailed(const std::string &reason) {
    failed_ = true;
    error_text_ = reason;
}

std::string MyRpcController::ErrorText() const { return error_text_; }

void MyRpcController::StartCancel() {
    // 目前未实现
}

bool MyRpcController::IsCanceled() const {
    // 目前未实现
    return false;
}

void MyRpcController::NotifyOnCancel(google::protobuf::Closure *callback) {
    // 目前未实现
}