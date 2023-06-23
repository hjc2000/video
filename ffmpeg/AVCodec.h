#pragma once
#include <Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	typedef ::AVCodecID AVCodecID;

	class AVCodec :public Wraper<::AVCodec>
	{
	public:// 生命周期
		AVCodec() {}
		AVCodec(::AVCodec* pAVCodec)
		{
			_pWrapedObj = pAVCodec;
		}
		AVCodec(const ::AVCodec* pAVCodec)
		{
			_pWrapedObj = (::AVCodec*)pAVCodec;
		}
		AVCodec(::AVCodec& refAVCodec)
		{
			_pWrapedObj = &refAVCodec;
		}
		AVCodec(const char* codec_name)
		{
			const ::AVCodec* ret = ::avcodec_find_encoder_by_name(codec_name);
			if (ret == nullptr)
				throw "查找编码器失败";
			_pWrapedObj = (::AVCodec*)ret;
		}
	};
}
