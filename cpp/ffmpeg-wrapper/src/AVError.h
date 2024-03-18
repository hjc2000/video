#pragma once
#include<ErrorCode.h>
#include<Exception.h>
#include<define.h>
#include<exception>
#include<format>
#include<include_ffmpeg.h>
#include<iostream>
#include<sstream>
#include<string>

using std::exception;
using std::string;
using std::stringstream;
using std::endl;

namespace video
{
	class FFmpegException : public jc::Exception
	{
	public:
		FFmpegException(string msg) :Exception(msg)
		{
		}

		FFmpegException(string msg, int err_code) :
			Exception(make_ffmpeg_error_string(msg, err_code))
		{
		}

	private:
		static string make_ffmpeg_error_string(string msg, int err_code)
		{
			std::string error_str = std::format(
				"消息：{}。错误代码：{}。错误代码转换成的字符串：{} \n",
				msg,
				err_code,
				ToString((ErrorCode)err_code)
			);
			return error_str;
		};
	};
}