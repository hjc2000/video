# 调用 PowerShell 脚本并捕获输出到变量中
function(execute_pwsh_script script_path output_variable)
	# 使用 execute_process 调用 PowerShell
	execute_process(
		COMMAND pwsh -File ${script_path}
		OUTPUT_VARIABLE script_output
		ERROR_VARIABLE script_error
		RESULT_VARIABLE script_result
	)
  
	# 检查脚本是否成功执行
	if(NOT "${script_result}" STREQUAL "0")
		message(SEND_ERROR "执行 PowerShell 脚本失败: ${script_error}")
	else()
		# 成功执行，将输出存储到提供的变量中
		set(${output_variable} "${script_output}" PARENT_SCOPE)
	endif()
endfunction()
