cmake_minimum_required(VERSION 3.30)
project(Test)

set(CMAKE_CXX_STANDARD 20)

# 设置 Qt 自动生成配置
set(CMAKE_AUTOMOC ON)  # 启用 MOC 处理
set(CMAKE_AUTOUIC ON)  # 启用 UIC 处理
set(CMAKE_AUTORCC ON)  # 启用 RCC 处理

# 添加include目录到包含路径
include_directories(include)

# 查找 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# 设置源文件
set(SOURCES
    src/main.cpp
    src/chatwindow.cpp
    src/gamewindow.cpp
)

# 设置头文件
set(HEADERS
    include/chatwindow.h
    include/gamewindow.h
    include/general.h
)

# 创建可执行文件
add_executable(Test ${SOURCES} ${HEADERS})

# 链接 Qt 库
target_link_libraries(Test PRIVATE Qt6::Widgets Qt6::Network)

# 添加头文件目录
target_include_directories(Test PRIVATE include)

