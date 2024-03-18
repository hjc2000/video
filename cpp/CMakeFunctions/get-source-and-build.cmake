# 更新仓库或克隆仓库。
# 首先检查 dst_dir 中是否已经有指定的仓库了，如果有且 should_update 为 TRUE，则进入进行 git pull 更新。
# 如果没有仓库，则进行克隆。
#
# 参数：
#   dst_dir - 要将仓库存放到的目的地址
#   repo_url - 仓库 URL
#   branch - 要克隆的分支
#   should_update - 如果为 TRUE，则对已存在的仓库执行 git pull 更新；如果为 FALSE，则跳过更新
#
# 使用例子：
# update_or_clone_repo(${CMAKE_SOURCE_DIR} https://gitee.com/Qianshunan/x264.git master FALSE)
function(update_or_clone_repo dst_dir repo_url branch should_update)
    # 从 URL 中提取仓库名称
    string(REGEX MATCH "/([^/]+)\\.git$" _ ${repo_url})
    set(repo_name "${CMAKE_MATCH_1}")

    # 构建完整的目标路径
    set(full_target_path "${dst_dir}/${repo_name}")

    # 检查目标目录是否存在
    if(IS_DIRECTORY "${full_target_path}")
        message(STATUS "Repository ${repo_name} exists.")

        # 根据 should_update 决定是否进行更新
        if(should_update)
            message(STATUS "Updating repository ${repo_name}...")
            execute_process(
                COMMAND git pull origin ${branch}
                WORKING_DIRECTORY ${full_target_path}
                RESULT_VARIABLE git_result
                OUTPUT_VARIABLE git_output
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )

            if(NOT git_result EQUAL "0")
                message(FATAL_ERROR "Git pull failed: ${git_output}")
            endif()
        else()
            message(STATUS "跳过更新仓库： ${repo_name}.")
        endif()
    else()
        # 目录不存在，克隆仓库
        message(STATUS "Cloning repository ${repo_name}...")
        execute_process(
            COMMAND git clone -b ${branch} ${repo_url} ${repo_name}
            WORKING_DIRECTORY ${dst_dir}
            RESULT_VARIABLE git_result
            OUTPUT_VARIABLE git_output
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(NOT git_result EQUAL "0")
            message(FATAL_ERROR "Git clone failed: ${git_output}")
        endif()
    endif()
endfunction()