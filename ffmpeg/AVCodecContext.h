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
			m_pWrapedObj = ::avcodec_alloc_context3(codec);
			if (!m_pWrapedObj)
			{
				throw "avcodec_alloc_context3失败";
			}
		}
		AVCodecContext(const AVCodecContext& refAVCodecContext)
		{
			m_pWrapedObj = refAVCodecContext.m_pWrapedObj;
			_copyed = true;
		}
		~AVCodecContext()
		{
			if (!_copyed)
			{
				avcodec_free_context(&m_pWrapedObj);
			}
		}
	public://公共方法
		void avcodec_open2(FFmpeg::AVDictionary* dic = nullptr)
		{
			int ret = ::avcodec_open2(m_pWrapedObj, _codec, *dic);
			if (ret < 0)
				throw ret;
		}
	private:
		FFmpeg::AVCodec _codec;
		bool _copyed = false;
	};
}

