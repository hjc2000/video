#include "AVCodec.h"

shared_ptr<FFmpeg::AVCodec> FFmpeg::AVCodec::find_encoder_by_name(const char *name)
{
	const ::AVCodec *ret = ::avcodec_find_encoder_by_name(name);
	if (ret == nullptr)
	{
		throw "查找编码器失败";
	}
	else
	{
		return shared_ptr<FFmpeg::AVCodec>{new FFmpeg::AVCodec{ ret }};
	}
}

shared_ptr<FFmpeg::AVCodec> FFmpeg::AVCodec::find_decoder_by_id(AVCodecID id)
{
	::AVCodec *ret = (::AVCodec *)::avcodec_find_decoder(id);
	if (!ret)
	{
		throw "查找编码器失败";
	}
	else
	{
		return shared_ptr<FFmpeg::AVCodec>{new FFmpeg::AVCodec{ ret }};
	}
}

void FFmpeg::AVCodec::operator=(const FFmpeg::AVCodec &ref)
{
	w = ref.w;
}
