cmake_minimum_required(VERSION 3.15)

# 设置c++版本
set(CMAKE_CXX_STANDARD 14)
project(tpc)

# 头文件路径
set(INC_DIR ./include /usr/local/include)
# 库路径
set(LINK_DIR /usr/local/lib)

# 需要编译的源文件列表
set(SRC_LIST ./src/trackCDLL.cpp ./src/rectangle.cpp ./src/node.cpp ./src/rtree.cpp ./src/trackC.cpp)
# 需要链接的库列表
set(LIB_LIST armadillo)

# 设置头文件目录
include_directories(${INC_DIR})
# 设置库目录
link_directories(${LINK_DIR})
# 设置要链接的库
link_libraries(${LIB_LIST})

# 生成可执行文件
add_library(tpc SHARED ${SRC_LIST})

# 完成库的链接
target_link_libraries(tpc ${LIB_LIST})
