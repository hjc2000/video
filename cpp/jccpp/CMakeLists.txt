if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName jccpp)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

# 编译目标
add_library(${ProjectName} SHARED)
target_add_source_recurse(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_and_install_headers_recurse(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
add_and_install_include_dir(${ProjectName} ${libs_path}/nlohmann-json/include/)

# 安装规则
if(1)
    # 安装编译出的静态库
    install_target_to_standard_paths(${ProjectName})
endif()
