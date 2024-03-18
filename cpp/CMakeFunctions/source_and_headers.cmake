# 将指定的头文件文件夹添加给目标，然后定义安装规则，安装的时候会安装该文件夹内的头文件，
# 并且会保持原来的目录结构。
function(add_and_install_include_dir target_name include_dir)
    target_include_directories(${target_name} PUBLIC ${include_dir})
    install_from_dir(${include_dir} include "*.h*")
endfunction()





# 递归访问 include_dir 及其所有子文件夹，收集所有含有头文件的文件夹，
# 然后分别为它们调用 add_and_install_include_dir
#
# 将指定的头文件文件夹添加给目标，然后定义安装规则，安装的时候会安装该文件夹内的头文件，
# 并且会保持原来的目录结构。
function(add_and_install_include_dir_recurse target_name include_dir)
    append_include_dirs_to_list(${include_dir} include_dirs)

    foreach(include_dir ${include_dirs})
        add_and_install_include_dir(${target_name} ${include_dir})
    endforeach()
endfunction()





# 递归访问 source_path，收集所有源文件，然后添加到目标中
function(target_add_source_recurse target_name source_path)
    append_source_files_to_list(${source_path}/ source_files)
    target_sources(${target_name} PRIVATE ${source_files})
endfunction()
