#include "AVCodec.h"

FFmpeg::AVCodec FFmpeg::AVCodec::find_encoder_by_name(const char *name)
{
	const ::AVCodec *ret = ::avcodec_find_encoder_by_name(name);
	if (ret == nullptr)
		throw "查找编码器失败";
	return ret;
}

FFmpeg::AVCodec FFmpeg::AVCodec::find_decoder_by_id(AVCodecID id)
{
	::AVCodec *ret = (::AVCodec *)::avcodec_find_decoder(id);
	if (!ret)
		throw "查找编码器失败";
	else
		return ret;
}

void FFmpeg::AVCodec::operator=(const FFmpeg::AVCodec &ref)
{
	_pWrapedObj = ref._pWrapedObj;
}
