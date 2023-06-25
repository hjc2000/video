#pragma once
#include<include_ffmpeg.h>
#include <Wraper.h>

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
			// 将 const 变量的指针强制转换为非 const 变量的指针。
			// 这么做是为了兼容包装类。虽然这么做了，但是不要去修改这个指针指向的变量的内容
			_pWrapedObj = (::AVCodec*)ret;
		}
		AVCodec(AVCodecID id)
		{
			::AVCodec* ret = (::AVCodec*)::avcodec_find_decoder(id);
			if (!ret)
				throw "查找编码器失败";
			else
				_pWrapedObj = ret;
		}
	};
}
