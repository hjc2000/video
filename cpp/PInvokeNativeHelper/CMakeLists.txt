if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName PInvokeNativeHelper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_src)

add_library(${ProjectName} SHARED)

# 将资源目录用预定义传给 C++ 程序
target_compile_definitions(${ProjectName} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")

target_import_src(${ProjectName})
target_link_libraries(${ProjectName} PUBLIC jccpp ffmpeg-wrapper)
