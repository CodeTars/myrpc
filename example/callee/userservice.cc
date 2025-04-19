#include <iostream>
#include <string>
#include "rpcapplication.h"
#include "rpcprovider.h"
#include "user.pb.h"

class UserServiceRPC : public example::UserService {
public:
    // 这是Login方法的实现
    bool Login(const std::string& name, const std::string& pwd) {
        std::cout << "========================" << std::endl;
        std::cout << "doing login request! " << std::endl;
        std::cout << "name: " << name << ", pwd: " << pwd << std::endl;
        std::cout << "login success!" << std::endl;
        std::cout << "========================" << std::endl;
        return true;
    }
    // 实现了UserService的Login RPC方法
    void Login(google::protobuf::RpcController* controller,
               const ::example::LoginRequest* request,
               ::example::LoginResponse* response,
               ::google::protobuf::Closure* done) override {
        const std::string& name = request->name();
        const std::string& pwd = request->pwd();
        // 调用Login方法进行登录验证
        bool ret = Login(name, pwd);
        if (ret) {
            example::ResultCode* rc = response->mutable_code();
            rc->set_errcode(0);
            rc->set_errmsg("ok");
            response->set_success(true);
        } else {
            example::ResultCode* rc = response->mutable_code();
            rc->set_errcode(-1);
            rc->set_errmsg("failed");
            response->set_success(false);
        }
        // 执行完毕，调用done回调【注意：done是一个Closure对象】
        // 这里的done是一个Closure对象，表示RPC调用完成后的回调
        done->Run();
    }
};

int main(int argc, char** argv) {
    // 初始化RPC应用程序
    RPCApplication::Init(argc, argv);

    // 将RPC服务发布到RPC节点上
    RPCProvider provider;
    UserServiceRPC user_service_rpc;
    provider.NotifyService(&user_service_rpc);

    // 启动一个rpc服务发布节点 Run以后，进程会进入阻塞状态
    provider.Run();
    return 0;
}