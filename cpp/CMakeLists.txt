# 基础配置
cmake_minimum_required (VERSION 3.8)
project ("video")

# 包含子项目。
add_subdirectory(${CMAKE_SOURCE_DIR}/ffmpeg-wrapper)
add_subdirectory(${CMAKE_SOURCE_DIR}/tsduck)
add_subdirectory(${CMAKE_SOURCE_DIR}/sdl2-wrapper)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_SOURCE_DIR}/ts-dumper)
add_subdirectory(${CMAKE_SOURCE_DIR}/avmixer)
add_subdirectory(${CMAKE_SOURCE_DIR}/check-avstream)