#include "test.pb.h"
#include <iostream>
#include <string>

int main() {
    test::Person person;
    person.set_name("Brain");
    person.set_age(10);
    person.set_email("Brain@test.com");

    std::string str;
    person.SerializeToString(&str);
    std::cout << "Serialized string: " << str << std::endl;
    std::cout << person.GetDescriptor()->DebugString() << std::endl;
    std::cout << person.GetDescriptor()->name() << std::endl;
    std::cout << person.GetDescriptor()->full_name() << std::endl;

    test::LoginRequest login_request;
    login_request.set_name("Steve");
    login_request.set_pwd("123456");

    login_request.SerializeToString(&str);
    test::LoginRequest login_request2;
    login_request2.ParseFromString(str);
    std::cout << "LoginRequest name: " << login_request2.name() << std::endl;
    std::cout << "LoginRequest pwd: " << login_request2.pwd() << std::endl;

    test::LoginResponse login_response;
    test::ResultCode *code = login_response.mutable_code();
    code->set_errcode(0);
    code->set_errmsg("ok");
    login_response.set_success(true);
    std::cout << login_response.SerializeAsString() << std::endl;

    return 0;
}