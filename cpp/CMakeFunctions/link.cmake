# 在 search_path 中递归查找 so_name 指定的 so 文件。首先查找具有单段版本号的，
# 找到了就链接上，然后返回。如果没有找到，就查找无版本号的，找到了就链接上，如果
# 还是没找到，就直接返回，不执行链接。
#
# 参数：
#   target_name - 目标名
#   so_name - 要链接的 so 的名称。不要带后缀。
#   search_path - 搜索 so 文件的路径。将会在此路径下递归搜索。
#   success - out 参数。用来指示是否成功找到 so 并链接上。
function(target_link_so target_name so_name search_path success)
    # 定义查找库的模式，首先是单段版本号的.so，然后是无版本号的.so
    set(patterns
        # 优先使用带单段版本号的
        "${so_name}.so.[0-9]"
        "lib${so_name}.so.[0-9]"

        # 其次使用不带版本号的
        "${so_name}.so"
        "lib${so_name}.so"
    )
    set(found_lib "")
    set(${success} FALSE PARENT_SCOPE)

    # 递归地搜索库文件
    foreach(pattern ${patterns})
        file(GLOB_RECURSE found_libs RELATIVE "${search_path}" "${search_path}/${pattern}")
        list(LENGTH found_libs found_count)
        if(found_count GREATER 0)
            list(GET found_libs 0 found_lib)
            break() # 找到第一个匹配的库就退出循环
        endif()
    endforeach()

    if(NOT found_lib STREQUAL "")
        # 如果找到了库，构造完整路径并链接到目标上
        set(full_path "${search_path}/${found_lib}")
        message(STATUS "Linking ${target_name} with ${full_path}")
        target_link_libraries(${target_name} PUBLIC "${full_path}")
        set(${success} TRUE PARENT_SCOPE)
    endif()
endfunction()



# 在 search_path 中递归查找静态库并链接。
# 首先查找 .a 文件，找到了就链接上，然后返回。找不到再查找 .lib。找到了就链接上，然后返回，还找不到就
# 直接返回，不链接。
#
# 虽然说是链接到静态库，但是一样可以用来链接 windows 中的 dll 的导入 lib。
#
# 参数：
#   target_name - 目标名
#   lib_name - 要链接的 .lib 或 .a 的名称。不要带后缀。
#   search_path - 搜索 .lib 或 .a 文件的路径。将会在此路径下递归搜索。
#   success - out 参数。用来指示是否成功找到 .lib 或 .a 并链接上。
function(target_link_static_lib target_name lib_name search_path success)
    # 定义查找库的模式，首先是.a，然后是.lib
    set(patterns
        # 优先使用动态库
        "${lib_name}.dll.a"
        "${lib_name}.dll.lib"
        "lib${lib_name}.dll.a"
        "lib${lib_name}.dll.lib"

        # 动态库找不到再使用静态库
        "${lib_name}.a"
        "${lib_name}.lib"
        "lib${lib_name}.a"
        "lib${lib_name}.lib"
    )
    set(found_lib "")
    set(${success} FALSE PARENT_SCOPE)

    # 递归地搜索库文件
    foreach(pattern ${patterns})
        file(GLOB_RECURSE found_libs RELATIVE "${search_path}" "${search_path}/${pattern}")
        list(LENGTH found_libs found_count)
        if(found_count GREATER 0)
            list(GET found_libs 0 found_lib)
            break() # 找到第一个匹配的库就退出循环
        endif()
    endforeach()

    if(NOT found_lib STREQUAL "")
        # 如果找到了库，构造完整路径并链接到目标上
        set(full_path "${search_path}/${found_lib}")
        message(STATUS "Linking ${target_name} with ${full_path}")
        target_link_libraries(${target_name} PUBLIC "${full_path}")
        set(${success} TRUE PARENT_SCOPE)
    endif()
endfunction()




# 自动链接到库。
# 首先尝试链接到 so，如果失败，尝试链接到静态库（包括 windows 中 dll 的导入 lib）。
# 参数：
#   target_name - 目标名。
#   lib_name - 库的名称。不要带后缀。
#   search_path - 递归搜索库的路径。
function(target_auto_link_lib target_name lib_name search_path)
    # 尝试链接共享库
    set(success_so)
    target_link_so(${target_name} ${lib_name} ${search_path} success_so)
    if(success_so)
        return()
    endif()

    # 继续尝试链接静态库
    set(success_static)
    target_link_static_lib(${target_name} ${lib_name} ${search_path} success_static)
    if(success_static)
        return()
    endif()

    message(WARNING "Failed to auto-link any library version for ${lib_name} in ${search_path}")
endfunction()
