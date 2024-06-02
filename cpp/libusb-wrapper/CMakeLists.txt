if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName libusb-wrapper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_libusb)
include(target_import_jccpp)
include(target_import_base)

# 定义生成规则
add_library("${ProjectName}")
target_import_src(${ProjectName})
target_import_libusb(${ProjectName} PUBLIC)
target_import_jccpp(${ProjectName} PUBLIC)
target_import_base(${ProjectName} PUBLIC)
