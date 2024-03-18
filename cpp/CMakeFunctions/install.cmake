# 定义一个函数来自动安装目标到标准的安装路径
# 这个函数确保可执行文件、动态库、静态库被安装到合适的目录中
#
# 参数:
#   target_name - 需要安装的目标名称。这应该是之前使用 add_executable 或 add_library 创建的目标。
#
function(install_target_to_standard_paths target_name)
    # 查询目标类型
    get_target_property(target_type ${target_name} TYPE)

    # 对于可执行文件和动态库 (共享库和DLL)，设置正确的安装路径
    if(target_type STREQUAL "EXECUTABLE" OR target_type STREQUAL "SHARED_LIBRARY")
        install(TARGETS ${target_name}
                RUNTIME DESTINATION bin # 可执行文件和DLL到 bin
                LIBRARY DESTINATION lib # 共享库 (.so) 到 lib
                )
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        # 静态库 (.a) 安装到 lib 目录
        install(TARGETS ${target_name} ARCHIVE DESTINATION lib)
    else()
        message(WARNING "Unsupported target type: ${target_type} for target: ${target_name}.")
    endif()

    # 注意：这里我们假设 DLL 作为运行时目标处理，因此它们将被安装到 bin 目录。
    # 如果目标有其他特定的文件需要安装，你可能需要扩展这个函数来处理那些情况。
endfunction()



# 定义安装规则。将一个文件夹的内容安装到安装目录。
# 参数：
#   src_dir - 此文件夹内的内容将被安装
#   dst_dir - 安装到的目标路径（相对于CMAKE_INSTALL_PREFIX）
#   pattern - 通配符
function(install_from_dir src_dir dst_dir pattern)
    if(NOT PATTERN)
        # 如果没有提供 PATTERN，设置为匹配所有文件
        set(PATTERN "*")
    endif()

    install(
        DIRECTORY ${src_dir}/
        DESTINATION ${dst_dir}
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING
        PATTERN ${pattern} # 使用提供的模式，或默认匹配所有文件
    )
endfunction()


# * 配置时会将 dll 复制到构建目录。
# * 定义了安装规则，安装时会将 dll 安装到安装目录中的 bin 目录。
function(install_dll_from_dir dll_dir)
    copy_from_dir(${dll_dir}/ ${CMAKE_CURRENT_BINARY_DIR} "*.dll")
    install_from_dir(${dll_dir}/ bin "*.dll")
endfunction()
