add_executable(
    consumer
    ../user.pb.cc
    calluserservice.cc
)

target_link_libraries(consumer myrpc)