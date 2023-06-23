#pragma once
#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVFrame :public Wraper<::AVFrame>
	{
	public:
		AVFrame()
		{
			_pWrapedObj = av_frame_alloc();
			if (!_pWrapedObj)
				throw "av_frame_alloc 失败";
		}
		AVFrame(::AVFrame* p)
		{
			_pWrapedObj = p;
		}
		~AVFrame()
		{
			if (should_dispose())
			{
				cout << "AVFrame 析构" << endl;
				av_frame_free(&_pWrapedObj);
			}
		}

	public:
		void av_frame_get_buffer(int align)
		{
			int ret = ::av_frame_get_buffer(_pWrapedObj, align);
			if (ret < 0)
				throw ret;
		}

		void av_frame_make_writable()
		{
			int ret= ::av_frame_make_writable(_pWrapedObj);
			if (ret < 0)
				throw ret;
		}
	};
}
