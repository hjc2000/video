#pragma once
#include<include_ffmpeg.h>
#include <Wraper.h>

namespace FFmpeg
{
	typedef ::AVCodecID AVCodecID;

	class AVCodec :public Wraper<::AVCodec>
	{
	public:// 构造函数、析构函数
		AVCodec() {}
		AVCodec(const ::AVCodec *p)
		{
			_pWrapedObj = (::AVCodec *)p;
		}
		AVCodec(::AVCodec &ref)
		{
			_pWrapedObj = &ref;
		}

	public:// 工厂函数
		static FFmpeg::AVCodec find_encoder_by_name(const char *name)
		{
			const ::AVCodec *ret = ::avcodec_find_encoder_by_name(name);
			if (ret == nullptr)
				throw "查找编码器失败";
			return ret;
		}

		static FFmpeg::AVCodec find_decoder_by_id(AVCodecID id)
		{
			::AVCodec *ret = (::AVCodec *)::avcodec_find_decoder(id);
			if (!ret)
				throw "查找编码器失败";
			else
				return ret;
		}

	public:
		void operator=(const FFmpeg::AVCodec &ref)
		{
			_pWrapedObj = ref._pWrapedObj;
		}
	};
}
