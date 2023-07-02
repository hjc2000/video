#pragma once
#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVFrame :public Wraper<::AVFrame>
	{
		#pragma region 生命周期
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
				cout << "AVFrame 释放" << endl;
				av_frame_free(&_pWrapedObj);
			}
		}
		#pragma endregion

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

		void unref()
		{
			::av_frame_unref(_pWrapedObj);
		}

		void copy_image_to(uint8_t *dst_data[4], int dst_linesizes[4])
		{
			av_image_copy(dst_data, dst_linesizes,
				(const uint8_t **)(_pWrapedObj->data), _pWrapedObj->linesize,
				(FFmpeg::AVPixelFormat)_pWrapedObj->format, _pWrapedObj->width, _pWrapedObj->height);
		}
	};
}
