#pragma once
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVCodecContext;

	/// <summary>
	/// 配合 AVFrame 使用。封装字节数组，用来储存一帧图像
	/// </summary>
	class ImageBuffer
	{
	public:
		ImageBuffer(FFmpeg::AVCodecContext ctx, int align);

	public:
		uint8_t *_pointers[4];
		int _linesizes[4];
		int _size;
	};
}

