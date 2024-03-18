setup_project(ffmpeg-wrapper)

function(setup_target target_name)
    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

    # 将资源目录用预定义传给 C++ 程序
    target_compile_definitions(${target_name} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")

    add_and_install_include_dir_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir(${target_name} ${libs_path}/ffmpeg/include/)
    add_and_install_include_dir(${target_name} ${libs_path}/sdl2/include/SDL2)

    target_link_libraries(${target_name} PUBLIC jccpp libtsduck)
    target_auto_link_lib(${target_name} libavcodec ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavdevice ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavfilter ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavformat ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavutil ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libpostproc ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libswresample ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libswscale ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} SDL2 ${libs_path}/sdl2/lib/)
endfunction()


# 定义生成规则
if(1)
    # 生成可执行文件
    add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    setup_target(${ProjectName})

    # 生成静态库
    add_library("lib${ProjectName}" STATIC)
    setup_target("lib${ProjectName}")
endif()


# 定义安装规则
if(1)
    install_dll_from_dir(${libs_path}/ffmpeg/bin/)
    install_dll_from_dir(${libs_path}/sdl2/bin/)

    # 安装本项目编译出的目标
    install_target_to_standard_paths("lib${ProjectName}")
    install_target_to_standard_paths(${ProjectName})
endif()
