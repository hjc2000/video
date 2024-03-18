#pragma once
#include<OutputFormat.h>
#include<AVIOContextWrapper.h>

namespace video
{
	class CustomOutputFormatContext :public OutputFormat
	{
	public:
		/**
		 * @brief
		 * @param url 这里的 url 不是用来创建文件的，而是让 ffmpeg 根据后缀名分析封装格式的。
		 * @param io_context
		*/
		CustomOutputFormatContext(char const *url, shared_ptr<AVIOContextWrapper> io_context);

		~CustomOutputFormatContext();

	private:
		shared_ptr<AVIOContextWrapper> _io_context;
	};
}