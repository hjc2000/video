if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName test)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_boost)
include(target_import_qt)
include(target_import_jccpp)

add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_compile_definitions(${ProjectName} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")
target_import_src(${ProjectName})
target_import_boost_asio(${ProjectName} PUBLIC)
target_import_qt_core(${ProjectName} PUBLIC)
target_import_jccpp(${ProjectName} PUBLIC)
target_link_libraries(${ProjectName} PUBLIC
    tsduck 
    ffmpeg-wrapper 
    sdl2-wrapper 
)
