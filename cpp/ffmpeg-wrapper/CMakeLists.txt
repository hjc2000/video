if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName ffmpeg-wrapper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()



include(target_import_ffmpeg)

# 编译为动态库
add_library(${ProjectName} SHARED)
target_import_src(${ProjectName})
target_link_libraries(${ProjectName} PUBLIC jccpp)
target_import_ffmpeg(${ProjectName})
