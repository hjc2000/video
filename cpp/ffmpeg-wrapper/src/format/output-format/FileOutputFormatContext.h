#pragma once
#include<OutputFormatBase.h>
#include<functional>
#include<include_ffmpeg.h>
#include<iostream>
#include<string>

using std::function;
using std::string;

namespace video
{
	class AVCodecContextWrapper;

	/**
	 * @brief 将封装写入文件。
	*/
	class FileOutputFormatContext :public OutputFormatBase
	{
	public:
		FileOutputFormatContext(const char *url);

		~FileOutputFormatContext();
	};
}