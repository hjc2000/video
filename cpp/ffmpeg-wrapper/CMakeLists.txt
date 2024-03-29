if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName ffmpeg-wrapper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include(${CMAKE_SOURCE_DIR}/cmake-import-helper/import-src.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-import-helper/import-ffmpeg.cmake)

function(setup_target target_name)
    target_import_src(${target_name})
    target_link_libraries(${target_name} PUBLIC jccpp)
    target_import_ffmpeg(${target_name})
endfunction()


# 定义生成规则
if(1)
    add_library("${ProjectName}" SHARED)
    setup_target("${ProjectName}")
endif()


# 定义安装规则
if(1)
    # 安装本项目编译出的目标
    install_target_to_standard_paths(${ProjectName})
endif()
