cmake_minimum_required(VERSION 3.31)
project(MyMemory)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
add_executable(MyMemory  main.cpp ${SRC_LIST})

target_include_directories(MyMemory
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src/include   # 内部头文件目录
        ${PROJECT_SOURCE_DIR}/lib/include
)