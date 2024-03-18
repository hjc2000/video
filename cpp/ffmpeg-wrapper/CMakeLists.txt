setup_project(ffmpeg-wrapper)

function(setup_target target_name)
    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir(${target_name} ${libs_path}/ffmpeg/include/)

    target_link_libraries(${target_name} PUBLIC jccpp tsduck)
    target_auto_link_lib(${target_name} libavcodec ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavdevice ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavfilter ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavformat ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavutil ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libpostproc ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libswresample ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libswscale ${libs_path}/ffmpeg/lib/)
endfunction()


# 定义生成规则
if(1)
    # 生成静态库
    add_library("${ProjectName}" STATIC)
    setup_target("${ProjectName}")
endif()


# 定义安装规则
if(1)
    # 安装本项目编译出的目标
    install_target_to_standard_paths(${ProjectName})
endif()