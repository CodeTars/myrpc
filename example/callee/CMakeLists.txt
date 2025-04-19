add_executable(
    provider
    userservice.cc
    ../user.pb.cc
)

target_link_libraries(provider myrpc)