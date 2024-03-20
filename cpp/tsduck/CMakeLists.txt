if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName tsduck)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

function(setup_target target_name)
    # 将资源目录用预定义传给 C++ 程序
    target_compile_definitions(${target_name} PRIVATE TS_NO_SRT=1)
    target_compile_definitions(${target_name} PRIVATE TS_NO_RIST=1)

    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    target_link_libraries(${target_name} PUBLIC jccpp)
endfunction()


# 定义生成规则
if(1)
    add_library("${ProjectName}" SHARED)
    setup_target("${ProjectName}")
endif()


# 定义安装规则
if(1)
    install_target_to_standard_paths(${ProjectName})
endif()