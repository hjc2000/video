#pragma once
#include<OutputFormat.h>
#include<functional>
#include<ffmpeg-wrapper/base_include.h>
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