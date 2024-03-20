# 基础配置
if(1)
	cmake_minimum_required (VERSION 3.8)
	project ("video")
	
	# 设置CMake构建时使用的线程数
	set(CMAKE_BUILD_PARALLEL_LEVEL 12)
endif()

if(1)
	# 安装时是否安装头文件
	option(option_install_headers true)
endif()


# 导入我的 cmake 函数
if(1)
	include(${CMAKE_SOURCE_DIR}/CMakeFunctions/install.cmake)
	include(${CMAKE_SOURCE_DIR}/CMakeFunctions/collect.cmake)
	include(${CMAKE_SOURCE_DIR}/CMakeFunctions/link.cmake)
	include(${CMAKE_SOURCE_DIR}/CMakeFunctions/copy.cmake)
	include(${CMAKE_SOURCE_DIR}/CMakeFunctions/source_and_headers.cmake)
	include(${CMAKE_SOURCE_DIR}/CMakeFunctions/get-source-and-build.cmake)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# 库仓库的路径
if(WIN32)
	set(libs_path "$ENV{cpp_lib_build_scripts_path}/msys/.libs" CACHE STRING "库仓库路径")
elseif(UNIX AND NOT APPLE)
	# platform 的值在预设里设置
	set(libs_path "$ENV{HOME}/cpp-lib-build-scripts/${platform}/.libs" CACHE STRING "库仓库路径")

	list(
		APPEND CMAKE_INSTALL_RPATH
		"$ORIGIN/../lib"
		"$ORIGIN/../usr/lib"
		"$ENV{HOME}/cpp-lib-build-scripts/${platform}/.total-install/lib"
		"$ENV{HOME}/cpp-lib-build-scripts/${platform}/.total-install/usr/lib"
	)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH true)

	set(CMAKE_EXE_LINKER_FLAGS 		"-Wl,-rpath-link,$ENV{HOME}/cpp-lib-build-scripts/${platform}/.total-install/lib:$ENV{HOME}/cpp-lib-build-scripts/${platform}/.total-install/usr/lib		${CMAKE_EXE_LINKER_FLAGS}" 		CACHE STRING "Linker flags for executables" 		FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS 	"-Wl,-rpath-link,$ENV{HOME}/cpp-lib-build-scripts/${platform}/.total-install/lib:$ENV{HOME}/cpp-lib-build-scripts/${platform}/.total-install/usr/lib		${CMAKE_SHARED_LINKER_FLAGS}" 	CACHE STRING "Linker flags for shared libraries"	FORCE)
endif()
message(STATUS "库仓库路径：${libs_path}")



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
