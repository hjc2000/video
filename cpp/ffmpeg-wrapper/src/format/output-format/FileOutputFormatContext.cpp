#include "FileOutputFormatContext.h"
#include<AVPacketWrapper.h>
#include<AVStreamWrapper.h>
#include<ErrorCode.h>

using namespace video;

video::FileOutputFormatContext::FileOutputFormatContext(const char *url)
{
	int ret = avformat_alloc_output_context2(&_wrapped_obj, nullptr, nullptr, url);
	if (ret < 0)
	{
		std::cerr << "FileOutputFormatContext 构造函数发生错误，错误代码："
			<< ToString((ErrorCode)ret)
			<< std::endl;

		throw jc::Exception();
	}

	ret = avio_open(&_wrapped_obj->pb, url, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		// 抛出异常前需要清理已经分配的资源。构造函数抛出异常，析构函数不会被自动调用。
		avformat_free_context(_wrapped_obj);

		std::cerr << "FileOutputFormatContext 构造函数发生错误，错误代码："
			<< ToString((ErrorCode)ret)
			<< std::endl;

		throw jc::Exception();
	}
}

video::FileOutputFormatContext::~FileOutputFormatContext()
{
	avio_closep(&_wrapped_obj->pb);
	avformat_free_context(_wrapped_obj);
}
