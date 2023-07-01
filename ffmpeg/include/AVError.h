#pragma once
#include<include_ffmpeg.h>
#include<string>

using std::string;

namespace FFmpeg
{
	std::string error_code_to_str(int error_code);

	class FFmpegException :public std::exception
	{
	public:
		FFmpegException(string msg) :exception(msg.c_str()) {}
	};
}

using FFmpeg::FFmpegException;