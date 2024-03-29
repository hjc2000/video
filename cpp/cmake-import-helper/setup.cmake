# 安装时是否安装头文件。默认会安装。
option(option_install_headers true)
# 不要第三方的头文件。默认不会安装。
option(option_do_not_install_third_party_headers true)



# 导入我的 cmake 函数
if(1)
	include(${CMAKE_SOURCE_DIR}/cmake-import-helper/CMakeFunctions/install.cmake)
	include(${CMAKE_SOURCE_DIR}/cmake-import-helper/CMakeFunctions/collect.cmake)
	include(${CMAKE_SOURCE_DIR}/cmake-import-helper/CMakeFunctions/link.cmake)
	include(${CMAKE_SOURCE_DIR}/cmake-import-helper/CMakeFunctions/source_and_headers.cmake)
	include(${CMAKE_SOURCE_DIR}/cmake-import-helper/CMakeFunctions/get-source-and-build.cmake)

	include(${CMAKE_SOURCE_DIR}/cmake-import-helper/import-src.cmake)
endif()




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
