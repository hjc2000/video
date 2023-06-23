#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVCodecContext :public Wraper<::AVCodecContext>
	{
	public:// 生命周期
		AVCodecContext() {}
		AVCodecContext(FFmpeg::AVCodec codec) :_codec(codec)
		{
			_pWrapedObj = ::avcodec_alloc_context3(codec);
			if (!_pWrapedObj)
			{
				throw "avcodec_alloc_context3失败";
			}
		}
		void DisposeWrapedObj()
		{
			avcodec_free_context(&_pWrapedObj);
		}
	public://公共方法
		void avcodec_open2(FFmpeg::AVDictionary* dic = nullptr)
		{
			int ret;
			if (dic)
				ret = ::avcodec_open2(_pWrapedObj, _codec, *dic);
			else
				ret = ::avcodec_open2(_pWrapedObj, _codec, nullptr);

			if (ret < 0)
				throw ret;
		}
	private:
		FFmpeg::AVCodec _codec;
		bool _copyed = false;
	};
}

