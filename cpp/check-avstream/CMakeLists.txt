if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName check-avstream)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_cli11)

add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_import_src(${ProjectName})
target_import_cli11(${ProjectName})
target_link_libraries(${ProjectName} PUBLIC
    jccpp 
    ffmpeg-wrapper 
)
