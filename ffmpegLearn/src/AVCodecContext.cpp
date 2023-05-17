#include "AVCodecContext.h"

FFmpeg::AVCodecContext::AVCodecContext(char *codecName)
{
	::AVCodec const *codec = avcodec_find_encoder_by_name(codecName);
	if (!codec)
	{
		cerr << "根据编码器名称获取 AVCodec 失败" << endl;
		throw -1;
	}
	_baseObj = avcodec_alloc_context3(codec);
	if (!_baseObj)
	{
		cerr << "创建编码器失败" << endl;
		throw -1;
	}
}