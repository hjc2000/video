# 定义一个函数 copy_files_from_directory
# 参数：
#   src_dirs - 源目录路径列表
#   dst_dir - 目标目录路径
#   pattern - 文件匹配模式，默认为复制所有文件
function(copy_from_dir src_dir dst_dir pattern)
    if(NOT pattern)
        # 如果没有提供 pattern，设置为匹配所有文件
        set(pattern "*")
    endif()

    # 搜索符合模式的文件
    file(GLOB_RECURSE files_to_copy ${src_dir}/${pattern})

    # 确保目标目录存在
    file(MAKE_DIRECTORY ${dst_dir})

    # 复制文件到目标目录
    file(COPY ${files_to_copy} DESTINATION ${dst_dir})
endfunction()
