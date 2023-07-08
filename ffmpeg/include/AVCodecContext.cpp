#include "AVCodecContext.h"
#include<AVDictionary.h>
#include<AVCodec.h>
#include<AVFrame.h>
#include<AVError.h>
#include<AVPacket.h>
#include<AVUtil.h>
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

void FFmpeg::AVCodecContext::open(FFmpeg::AVDictionary *dic)
{
	int ret;
	if (dic != nullptr)
	{
		ret = ::avcodec_open2(_pWrapedObj, _codec, *dic);
	}
	else
	{
		ret = ::avcodec_open2(_pWrapedObj, _codec, nullptr);
	}

	if (ret < 0)
	{
		throw Exception("AVCodecContext::open", ret);
	}
}

void FFmpeg::AVCodecContext::avcodec_send_frame(FFmpeg::AVFrame frame)
{
	::avcodec_send_frame(_pWrapedObj, frame);
}

int FFmpeg::AVCodecContext::avcodec_receive_packet(FFmpeg::AVPacket pkt)
{
	return ::avcodec_receive_packet(_pWrapedObj, pkt);
}

void FFmpeg::AVCodecContext::send_packet(FFmpeg::AVPacket packet)
{
	int ret = ::avcodec_send_packet(_pWrapedObj, packet);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::send_packet", ret);
	}
}

int FFmpeg::AVCodecContext::receive_frame(FFmpeg::AVFrame frame)
{
	return ::avcodec_receive_frame(_pWrapedObj, frame);
}

void FFmpeg::AVCodecContext::set_codec_param(AVCodecParameters *param)
{
	int ret = ::avcodec_parameters_to_context(_pWrapedObj, param);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::set_codec_param", ret);
	}
}
