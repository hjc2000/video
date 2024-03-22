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
            ${path}/*.hpp
        )

        set(${out_list} ${temp_list} PARENT_SCOPE)
    endfunction()


    collect_source_files_to_list(${path} temp_list)
    list(APPEND temp_list ${${out_list}})
    set(${out_list} ${temp_list} PARENT_SCOPE)
endfunction()
