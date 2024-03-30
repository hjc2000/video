if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName libusb-wrapper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include(target_import_libusb)

# 定义生成规则
add_library("${ProjectName}" SHARED)
target_import_src(${ProjectName})
target_import_libusb(${ProjectName})
target_link_libraries(${ProjectName} PUBLIC jccpp)
