# 将指定的头文件文件夹添加给目标，然后定义安装规则，安装的时候会安装该文件夹内的头文件，
# 并且会保持原来的目录结构。
function(add_and_install_include_dir target_name include_dir)
    target_include_directories(${target_name} PUBLIC ${include_dir})
    if(${option_install_headers})
        install_from_dir(${include_dir} include "*.h*")
    endif()
endfunction()



# 递归收集头文件，添加到查找路径，然后定义安装规则，安装时会将这些头文件
# 都安装到 include 目录下。
function(add_and_install_headers_recurse target src_dir)
    # 收集指定目录及其子目录下所有的头文件
    file(GLOB_RECURSE HEADERS "${src_dir}/*.h")

    # 获取所有头文件的目录并去重，以添加到编译时的包含目录中
    set(DIRS "")
    foreach(HEADER ${HEADERS})
        get_filename_component(DIR ${HEADER} DIRECTORY)
        list(APPEND DIRS ${DIR})
    endforeach()
    list(REMOVE_DUPLICATES DIRS)

    # 将收集到的目录添加到目标的编译时包含目录中
    target_include_directories(${target} PUBLIC ${DIRS})

    # 安装所有收集到的头文件到安装前缀的include目录下
    install(FILES ${HEADERS} DESTINATION include)
endfunction()





# 递归访问 source_path，收集所有源文件，然后添加到目标中
function(target_add_source_recurse target_name source_path)
    append_source_files_to_list(${source_path}/ source_files)
    target_sources(${target_name} PRIVATE ${source_files})
endfunction()
