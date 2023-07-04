#pragma once
#include<Wraper.h>
#include<AVUtil.h>
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
		~AVFrame() { Dispose(); }
		void Dispose() override;
		#pragma endregion

		#pragma region 工厂函数
	public:
		static FFmpeg::AVFrame create();
		#pragma endregion

	public:
		void av_frame_get_buffer(int align)
		{
			int ret = ::av_frame_get_buffer(_pWrapedObj, align);
			if (ret < 0)
				throw ret;
		}

		void make_writable()
		{
			int ret = ::av_frame_make_writable(_pWrapedObj);
			if (ret < 0)
				throw ret;
		}

		void unref()
		{
			::av_frame_unref(_pWrapedObj);
		}

		/// <summary>
		/// * 将图像复制到缓冲区。注意，此方法以来 AVFrame 中的参数，如果有某些情况下 AVFrame 中的
		/// 参数没有被设置，则无法使用此方法。
		/// * 音频和视频的包解码后都是得到 AVFrame，只有视频帧才能调用此方法
		/// </summary>
		/// <param name="dst_data"></param>
		/// <param name="dst_linesizes"></param>
		void copy_image_to(uint8_t *dst_data[4], int dst_linesizes[4])
		{
			av_image_copy(dst_data, dst_linesizes,
				(const uint8_t **)(_pWrapedObj->data), _pWrapedObj->linesize,
				(FFmpeg::AVPixelFormat)_pWrapedObj->format, _pWrapedObj->width, _pWrapedObj->height);
		}
	};
}
