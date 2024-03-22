if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName test)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

function(setup_target target_name)
    target_compile_definitions(${target_name} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")
    target_compile_definitions(${target_name} PRIVATE SDL_MAIN_HANDLED)

    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

    add_and_install_headers_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_include_dir(${target_name} ${libs_path}/boost/include/)

    target_link_libraries(${target_name} PUBLIC jccpp tsduck ffmpeg-wrapper sdl2-wrapper)
    if(WIN32)
        # 定义 windows API 版本
        target_compile_definitions(${target_name} PRIVATE -D_WIN32_WINNT=0x0A00)
        target_link_libraries(${target_name} PUBLIC ws2_32)
    endif()
endfunction()


# 定义生成规则
if(1)
    add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    setup_target(${ProjectName})
endif()


# 定义安装规则
if(1)
    install_dll_from_dir(${libs_path}/ffmpeg/bin/)
    install_target_to_standard_paths(${ProjectName})
endif()