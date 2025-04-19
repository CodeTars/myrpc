project(myrpc)

cmake_minimum_required(VERSION 3.15)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 设置头文件输出路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 设置库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(example)
add_subdirectory(src)