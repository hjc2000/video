#include "AVCodecContext.h"
#include<AVDictionary.h>
#include<AVCodec.h>
#include<AVFrame.h>
#include<AVError.h>
#include<AVPacket.h>
#include<AVUtil.h>
#include<AVDictionary.h>

FFmpeg::AVCodecContext::AVCodecContext(shared_ptr<FFmpeg::AVCodec> codec)
{
	_codec = codec;
	w = ::avcodec_alloc_context3(codec->w);
	if (!w)
	{
		throw Exception("FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec) 失败");
	}
}

FFmpeg::AVCodecContext::AVCodecContext(shared_ptr<FFmpeg::AVCodec> codec, AVCodecParameters *param, bool autoOpen)
{
	_codec = codec;
	w = ::avcodec_alloc_context3(codec->w);
	if (!w)
	{
		throw Exception("FFmpeg::AVCodecContext create(FFmpeg::AVCodec codec) 失败");
	}

	int ret = ::avcodec_parameters_to_context(w, param);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::create", ret);
	}
	else
	{
		if (autoOpen)
		{
			open();
		}
	}
}

void FFmpeg::AVCodecContext::Dispose()
{
	if (should_dispose())
	{
		cout << "AVCodecContext 释放" << endl;
		avcodec_free_context(&w);
	}
}

void FFmpeg::AVCodecContext::open(FFmpeg::AVDictionary *dic)
{
	int ret;
	if (dic != nullptr)
	{
		ret = ::avcodec_open2(w, _codec->w, *dic);
	}
	else
	{
		ret = ::avcodec_open2(w, _codec->w, nullptr);
	}

	if (ret < 0)
	{
		throw Exception("AVCodecContext::open", ret);
	}
}

void FFmpeg::AVCodecContext::avcodec_send_frame(shared_ptr<FFmpeg::AVFrame> frame)
{
	::avcodec_send_frame(w, frame->w);
}

int FFmpeg::AVCodecContext::avcodec_receive_packet(FFmpeg::AVPacket pkt)
{
	return ::avcodec_receive_packet(w, pkt);
}

void FFmpeg::AVCodecContext::send_packet(shared_ptr<FFmpeg::AVPacket> packet)
{
	int	ret = ::avcodec_send_packet(w, packet ? packet->w : nullptr);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::send_packet", ret);
	}
}

int FFmpeg::AVCodecContext::receive_frame(shared_ptr<FFmpeg::AVFrame> frame)
{
	return ::avcodec_receive_frame(w, frame->w);
}

void FFmpeg::AVCodecContext::set_codec_param(AVCodecParameters *param)
{
	int ret = ::avcodec_parameters_to_context(w, param);
	if (ret < 0)
	{
		throw Exception("AVCodecContext::set_codec_param", ret);
	}
}
