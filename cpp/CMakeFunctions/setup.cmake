# 安装项目
# 参数：
#   project_name - 项目名
# 为父作用域引入的变量：
#   ProjectName
#   CMAKE_CXX_STANDARD
#   CMAKE_C_STANDARD
macro(setup_project project_name)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName ${project_name})
    project(${project_name})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endmacro()




macro(setup_c_project project_name)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName ${project_name})
    project(${project_name} C)

    set(CMAKE_C_STANDARD 17)
endmacro()
