setup_project(sdl2-wrapper)

function(setup_target target_name)
    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

    # 将资源目录用预定义传给 C++ 程序
    target_compile_definitions(${target_name} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")

    add_and_install_include_dir_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir(${target_name} ${libs_path}/sdl2/include/SDL2)

    target_link_libraries(${target_name} PUBLIC jccpp libffmpeg-wrapper)
    target_auto_link_lib(${target_name} SDL2 ${libs_path}/sdl2/lib/)
endfunction()


# 定义生成规则
if(1)
    # 生成静态库
    add_library("${ProjectName}" STATIC)
    setup_target("${ProjectName}")
endif()


# 定义安装规则
if(1)
    install_dll_from_dir(${libs_path}/sdl2/bin/)

    # 安装本项目编译出的目标
    install_target_to_standard_paths(${ProjectName})
endif()