#pragma once
#include<ffmpeg-wrapper/wrapper/AVIOContextWrapper.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>

namespace video
{
	class IOContextOutputFormat :public OutputFormat
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="url">这里的 url 不是用来创建文件的，而是让 ffmpeg 根据后缀名分析封装格式的</param>
		/// <param name="io_context"></param>
		IOContextOutputFormat(std::string url, shared_ptr<AVIOContextWrapper> io_context);

		~IOContextOutputFormat();

	private:
		shared_ptr<AVIOContextWrapper> _io_context;
	};
}