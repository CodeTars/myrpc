#include "rpcapplication.h"
#include "rpcchannel.h"
#include "rpccontroller.h"
#include "user.pb.h"

int main(int argc, char** argv) {
    // 初始化RPC应用程序
    RPCApplication::Init(argc, argv);

    // 创建一个请求对象
    example::LoginRequest req;
    req.set_name("tom");
    req.set_pwd("123456");

    // stub是一个RPC存根对象，用于调用远程服务
    // stub的构造函数需要一个Channel对象，Channel对象用于与RPC节点进行通信
    example::LoginResponse resp;
    MyRpcController controller;
    MyRpcChannel channel;
    example::UserService_Stub stub(&channel);
    stub.Login(&controller, &req, &resp, nullptr);

    if (controller.Failed()) {
        std::cout << "RPC failed: " << controller.ErrorText() << std::endl;
        return -1;
    }

    // 打印响应结果
    std::cout << "RPC Login response success!" << std::endl;
    std::cout << "code: " << resp.code().errcode() << ", " << resp.code().errmsg() << std::endl;
    std::cout << "success: " << resp.success() << std::endl;

    return 0;
}