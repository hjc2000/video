#include "AVCodecContext.h"
#include<AVDictionary.h>

void FFmpeg::AVCodecContext::Dispose()
{
	if (should_dispose())
	{
		cout << "AVCodecContext 释放" << endl;
		avcodec_free_context(&_pWrapedObj);
	}
}

FFmpeg::AVCodecContext FFmpeg::AVCodecContext::create(FFmpeg::AVCodec codec)
{
	FFmpeg::AVCodecContext ctx;
	ctx._codec = codec;
	ctx._pWrapedObj = ::avcodec_alloc_context3(codec);
	if (!ctx._pWrapedObj)
	{
		throw Exception("FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec) 失败");
	}

	return ctx;
}

FFmpeg::AVCodecContext FFmpeg::AVCodecContext::create(FFmpeg::AVCodec codec, AVCodecParameters *param, bool autoOpen)
{
	FFmpeg::AVCodecContext ctx = create(codec);
	int ret = ::avcodec_parameters_to_context(ctx._pWrapedObj, param);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::create", ret);
	}
	else
	{
		if (autoOpen)
		{
			ctx.open();
		}

		return ctx;
	}
}

void FFmpeg::AVCodecContext::open(FFmpeg::AVDictionary dic)
{
	int ret = ::avcodec_open2(_pWrapedObj, _codec, dic);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::open", ret);
	}
}
