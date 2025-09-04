cmake_minimum_required(VERSION 3.30)
project(Test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

add_executable(Test src/main.cpp src/chatwindow.cpp src/gamewindow.cpp)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
target_link_libraries(Test PRIVATE Qt6::Widgets Qt6::Network)
