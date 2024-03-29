if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName a3)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

function(setup_target target_name)
    target_import_src(${target_name})
    target_link_libraries(${target_name} PUBLIC jccpp libusb-wrapper)
endfunction()


# 定义生成规则
if(1)
    # 生成可执行文件
    add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    setup_target(${ProjectName})

    add_library("lib${ProjectName}" SHARED)
    setup_target("lib${ProjectName}")
endif()
