#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<functional>
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
	class FileOutputFormat :public OutputFormat
	{
	public:
		FileOutputFormat(std::string url);
		~FileOutputFormat();
	};
}