setup_project(PInvokeNativeHelper)

function(setup_target target_name)
    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

    # 将资源目录用预定义传给 C++ 程序
    target_compile_definitions(${target_name} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")

    add_and_install_include_dir_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

    target_link_libraries(${target_name} PUBLIC jccpp libffmpeg-wrapper)
endfunction()


# 定义生成规则
if(1)
    # 生成可执行文件
    add_library(${ProjectName} SHARED)
    setup_target(${ProjectName})
endif()


# 定义安装规则
if(1)
    # 安装本项目编译出的目标
    install_target_to_standard_paths(${ProjectName})
endif()
