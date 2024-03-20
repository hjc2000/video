setup_project(libusb-wrapper)

function(setup_target target_name)
    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

    add_and_install_include_dir_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir(${target_name} ${libs_path}/libusb/include/)

    target_link_libraries(${target_name} PUBLIC jccpp)
    target_auto_link_lib(${target_name} libusb-1.0 ${libs_path}/libusb/lib/)
endfunction()


# 定义生成规则
if(1)
    # 生成静态库
    add_library("${ProjectName}" SHARED)
    setup_target("${ProjectName}")
endif()


# 定义安装规则
if(1)
    install_dll_from_dir(${libs_path}/libusb/bin/)

    # 安装本项目编译出的目标
    install_target_to_standard_paths(${ProjectName})
endif()
