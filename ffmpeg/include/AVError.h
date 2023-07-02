#pragma once
#include<include_ffmpeg.h>
#include<string>

using std::string;

namespace FFmpeg
{
	std::string error_code_to_str(int error_code);

	enum ErrorType
	{
		/// <summary>
		/// 未知错误
		/// </summary>
		unknow,

		/// <summary>
		/// 到达文件尾
		/// </summary>
		eof = AVERROR_EOF,

		/// <summary>
		/// 输出当前不可用，用户必须先输入
		/// </summary>
		need_input = AVERROR(EAGAIN),

		/// <summary>
		/// 当前编码器未打开，或者它是一个解码器
		/// </summary>
		encoder_not_opened = AVERROR(EINVAL),
	};

	class Exception :public std::exception
	{
	public:
		Exception(string msg, ErrorType type = ErrorType::unknow) :exception(msg.c_str())
		{
			_type = type;
		}

		friend bool operator==(Exception e, ErrorType type);
		friend bool operator==(ErrorType type, Exception e);

	private:
		ErrorType _type = ErrorType::unknow;
	};

	bool operator==(Exception e, ErrorType type);
	bool operator==(ErrorType type, Exception e);
}

using FFmpeg::Exception;