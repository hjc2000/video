if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName jccpp)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_nlohmann_json)

# 编译目标
add_library(${ProjectName} SHARED)
target_import_src(${ProjectName})
target_import_nlohmann_json(${ProjectName})

# 平台特定的代码，独立于 src 之外
if(WIN32)
    target_add_source_recurse(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/win/)
    add_and_install_headers_recurse(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/win/)
endif()
