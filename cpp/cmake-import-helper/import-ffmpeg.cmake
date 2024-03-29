# 导入 ffmpeg 库
# 首先使用
# include(${CMAKE_SOURCE_DIR}/cmake-import-libs-helper/import-ffmpeg.cmake)
# 包含本文件，获得本函数的定义，然后调用，让目标导入 ffmpeg 库。
function(target_import_ffmpeg target_name)
    add_and_install_include_dir(${target_name} ${libs_path}/ffmpeg/include/)

    target_auto_link_lib(${target_name} libavcodec ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavdevice ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavfilter ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavformat ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libavutil ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libpostproc ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libswresample ${libs_path}/ffmpeg/lib/)
    target_auto_link_lib(${target_name} libswscale ${libs_path}/ffmpeg/lib/)

    install_dll_from_dir(${libs_path}/ffmpeg/bin/)
endfunction()
