if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName sdl2-wrapper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

include(target_import_sdl2)

function(setup_target target_name)
    # 将资源目录用预定义传给 C++ 程序
    target_compile_definitions(${target_name} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")
    target_compile_definitions(${target_name} PRIVATE SDL_MAIN_HANDLED)

    target_import_src(${target_name})
    target_import_sdl2(${target_name})
    target_link_libraries(${target_name} PUBLIC jccpp ffmpeg-wrapper)
endfunction()


# 定义生成规则
if(1)
    add_library("${ProjectName}" SHARED)
    setup_target("${ProjectName}")
endif()
