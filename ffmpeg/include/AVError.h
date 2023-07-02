#pragma once
#include<include_ffmpeg.h>
#include<string>
#include<iostream>

using std::string;

namespace FFmpeg
{
	std::string error_code_to_str(int error_code);

	enum ErrorType
	{
		/// <summary>
		/// 暂时没有放入此枚举类型的错误
		/// </summary>
		unknow,

		/// <summary>
		/// AVERROR_EOF
		/// 到达文件尾
		/// </summary>
		eof = AVERROR_EOF,

		/// <summary>
		/// AVERROR(EAGAIN)
		/// 1. 输出当前不可用，用户必须先输入
		/// 2. 输入当前不可用，因为上一次输入产生的输出还么被拿走
		/// </summary>
		eagain = AVERROR(EAGAIN),

		/// <summary>
		/// AVERROR(EINVAL)
		/// 1. 当前编码器未打开，或者它是一个解码器
		/// 2. 当前解码器未打开，或者它是一个编码器。或者此解码器需要冲洗
		/// </summary>
		encoder_not_opened = AVERROR(EINVAL),
	};

	class Exception :public std::exception
	{
		#pragma region 构造函数
	public:
		Exception(string msg) :exception(msg.c_str()) {}
		Exception(string err_source, int err_code) :exception(make_err_string(err_source, err_code).c_str()) {}
		#pragma endregion

	private:
		FFmpeg::ErrorType _type = FFmpeg::ErrorType::unknow;

		static string make_err_string(string err_source, int err_code)
		{
			return string{ "来自 " + err_source + " 的异常：" + FFmpeg::error_code_to_str(err_code) };
		}
	};

	std::ostream &operator<<(std::ostream &os, std::exception e);
}

