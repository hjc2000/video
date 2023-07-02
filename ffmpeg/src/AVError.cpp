#include <AVError.h>

std::string FFmpeg::error_code_to_str(int error_code)
{
	char err_buffer[1024];
	int result = ::av_strerror(error_code, err_buffer, sizeof(err_buffer));
	if (result >= 0)
		return std::string{err_buffer};
	else
		return "找不到该错误代码";
}
