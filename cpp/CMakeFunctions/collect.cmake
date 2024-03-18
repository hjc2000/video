# 递归收集 path 中的 .cpp, .c, .h 文件追加到 out_list。
# 参数：
#   path - 源文件所在的路径
#   out_list - 传入列表的变量名。
function(append_source_files_to_list path out_list)
    function(collect_source_files_to_list path out_list)
        file(GLOB_RECURSE
            temp_list
            ${path}/*.cpp
            ${path}/*.c
            ${path}/*.h
        )

        set(${out_list} ${temp_list} PARENT_SCOPE)
    endfunction()


    collect_source_files_to_list(${path} temp_list)
    list(APPEND temp_list ${${out_list}})
    set(${out_list} ${temp_list} PARENT_SCOPE)
endfunction()







# 从 path 递归收集所有含有头文件的文件夹，将这些文件夹的路径追加到 out_list 中。
#
# 注意，有的情况不能用。例如 ffmpeg 是分模块的，包含它的头文件得连着文件夹。例如：libavutil/xxxx.h
# 这样子的。这个时候如果递归收集头文件就会出错，因为要让头文件保持在文件夹内，不能进入文件夹把它收集出来。
function(append_include_dirs_to_list path out_list)
    function(collect_include_dirs path out_list)
        # 递归地收集所有.h文件
        file(GLOB_RECURSE ALL_HEADER_FILES "${path}/*.h")

        # 创建一个变量来存储目录
        set(DIRS "")

        # 遍历所有头文件
        foreach(HEADER ${ALL_HEADER_FILES})
            # 获取头文件的目录
            get_filename_component(DIR_PATH "${HEADER}" DIRECTORY)

            # 将目录添加到列表中
            list(APPEND DIRS ${DIR_PATH})
        endforeach()

        set(${out_list} ${DIRS} PARENT_SCOPE)
    endfunction()


    collect_include_dirs(${path} temp_list)
    list(APPEND temp_list ${${out_list}})

    # 移除列表中的重复项
    list(REMOVE_DUPLICATES temp_list)
    set(${out_list} ${temp_list} PARENT_SCOPE)
endfunction()
