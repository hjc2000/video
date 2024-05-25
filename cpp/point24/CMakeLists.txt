cmake_minimum_required(VERSION 3.8)
include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_boost)
include(target_import_qt)
include(target_import_jccpp)
include(target_import_base)

set(ProjectName point24)
project(${ProjectName})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_import_src(${ProjectName})
target_import_jccpp(${ProjectName} PUBLIC)
target_import_base(${ProjectName} PUBLIC)
