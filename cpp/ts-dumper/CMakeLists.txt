if(1)
    cmake_minimum_required(VERSION 3.8)

    set(ProjectName ts-dumper)
    project(${ProjectName})

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_C_STANDARD 17)
endif()

function(setup_target target_name)
    target_compile_definitions(${target_name} PRIVATE Predefine_ResourceDir="${CMAKE_CURRENT_SOURCE_DIR}/资源")
    target_compile_definitions(${target_name} PRIVATE SDL_MAIN_HANDLED)
    target_import_src(${target_name})
    
    add_and_install_third_party_include_dir(${target_name} ${libs_path}/cli11/include/)

    target_link_libraries(${target_name} PUBLIC jccpp tsduck)
endfunction()


# 定义生成规则
if(1)
    add_executable(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    setup_target(${ProjectName})
endif()
