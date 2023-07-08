#pragma once
#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class ImageBuffer;

	class AVFrame :public Wraper<::AVFrame>
	{
		#pragma region 生命周期
	public:
		AVFrame() {}
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
		void av_frame_get_buffer(int align);

		void make_writable();

		void unref();

		/// <summary>
		/// * 将图像复制到缓冲区。注意，此方法依赖 AVFrame 中的参数，如果有某些情况下 AVFrame 中的
		/// 参数没有被设置，则无法使用此方法。
		/// * 虽然音频和视频的包解码后都是得到 AVFrame，但只有视频帧的 AVFrame 才能调用此方法
		/// </summary>
		/// <param name="dst_data"></param>
		/// <param name="dst_linesizes"></param>
		void copy_image_to_arr(FFmpeg::ImageBuffer buffer);
	};
}
