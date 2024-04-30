# 基础配置
if(1)
	cmake_minimum_required (VERSION 3.8)
	project ("video")
	
	# 设置CMake构建时使用的线程数
	set(CMAKE_BUILD_PARALLEL_LEVEL 12)
endif()


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
	# 导出动态库的所有符号
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

	#add_compile_options(/wd26819)
	#add_compile_options(/wd26439)

	# 下面是如何禁用链接器警告
	# add_link_options(/ignore:4099 /ignore:4006)
endif()


# 包含子项目。
add_subdirectory(${CMAKE_SOURCE_DIR}/jccpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/PInvokeNativeHelper)
add_subdirectory(${CMAKE_SOURCE_DIR}/ffmpeg-wrapper)
add_subdirectory(${CMAKE_SOURCE_DIR}/tsduck)
add_subdirectory(${CMAKE_SOURCE_DIR}/a3)
add_subdirectory(${CMAKE_SOURCE_DIR}/libusb-wrapper)
add_subdirectory(${CMAKE_SOURCE_DIR}/sdl2-wrapper)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_SOURCE_DIR}/ts-dumper)
add_subdirectory(${CMAKE_SOURCE_DIR}/point24)
add_subdirectory(${CMAKE_SOURCE_DIR}/avmixer)
add_subdirectory(${CMAKE_SOURCE_DIR}/check-avstream)
