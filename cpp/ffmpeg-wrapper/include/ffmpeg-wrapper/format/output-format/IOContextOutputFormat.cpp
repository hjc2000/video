#include "ffmpeg-wrapper/format/output-format/IOContextOutputFormat.h"
#include<ErrorCode.h>

using namespace video;

IOContextOutputFormat::IOContextOutputFormat(std::string url, shared_ptr<AVIOContextWrapper> io_context)
{
	_io_context = io_context;
	int ret = avformat_alloc_output_context2(&_wrapped_obj, nullptr, nullptr, url.c_str());
	if (ret < 0)
	{
		std::cerr << CODE_POS_STR << ToString((ErrorCode)ret) << std::endl;
		throw jc::Exception();
	}

	_wrapped_obj->flags |= AVFMT_FLAG_CUSTOM_IO;
	_wrapped_obj->pb = *io_context;
}

IOContextOutputFormat::~IOContextOutputFormat()
{
	avformat_free_context(_wrapped_obj);
}
