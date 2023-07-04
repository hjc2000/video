#include "AVCodecContext.h"
#include<AVDictionary.h>

void FFmpeg::AVCodecContext::open(FFmpeg::AVDictionary dic)
{
	int ret = ::avcodec_open2(_pWrapedObj, _codec, dic);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::open", ret);
	}
}
