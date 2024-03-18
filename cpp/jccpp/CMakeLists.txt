setup_project(jccpp)

# 编译目标
add_library(${ProjectName} STATIC)
target_add_source_recurse(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_and_install_include_dir_recurse(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
add_and_install_include_dir(${ProjectName} ${libs_path}/nlohmann-json/include/)

# 安装编译出的静态库
install_target_to_standard_paths(${ProjectName})