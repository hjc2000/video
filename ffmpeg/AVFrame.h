#pragma once
#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVFrame :public Wraper<::AVFrame>
	{
	public:
		AVFrame() :Wraper() {}
		AVFrame(::AVFrame *p) :Wraper(p) {}
		AVFrame(::AVFrame &ref) :Wraper(ref) {}

		~AVFrame()
		{
			Dispose();
		}

		void Dispose() override
		{
			if (should_dispose())
			{
				cout << "AVFrame 析构" << endl;
				av_frame_free(&_pWrapedObj);
			}
		}

		#pragma region 工厂函数
	public:
		static FFmpeg::AVFrame create()
		{
			FFmpeg::AVFrame frame;
			frame._pWrapedObj = av_frame_alloc();
			if (!frame._pWrapedObj)
				throw "av_frame_alloc 失败";
			return frame;
		}
		#pragma endregion

	public:
		void av_frame_get_buffer(int align)
		{
			int ret = ::av_frame_get_buffer(_pWrapedObj, align);
			if (ret < 0)
				throw ret;
		}

		void av_frame_make_writable()
		{
			int ret = ::av_frame_make_writable(_pWrapedObj);
			if (ret < 0)
				throw ret;
		}
	};
}
