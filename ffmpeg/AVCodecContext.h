#pragma once
#include<Wraper.h>
#include<AVCodec.h>
#include<AVFrame.h>
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
				throw "avcodec_alloc_context3失败";
		}
		~AVCodecContext()
		{
			if (should_dispose())
			{
				cout << "AVCodecContext 析构" << endl;
				avcodec_free_context(&_pWrapedObj);
			}
		}

	public://公共方法
		void avcodec_open2(FFmpeg::AVDictionary dic = nullptr)
		{
			int ret = ::avcodec_open2(_pWrapedObj, _codec, dic);
			if (ret < 0)
				throw ret;
		}
		void avcodec_send_frame(FFmpeg::AVFrame frame)
		{
			::avcodec_send_frame(_pWrapedObj, frame);
		}
	private:
		FFmpeg::AVCodec _codec;
		bool _copyed = false;
	};
}

