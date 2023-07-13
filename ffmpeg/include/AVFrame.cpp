#include "AVFrame.h"
#include<AVUtil.h>
#include<ImageBuffer.h>

FFmpeg::AVFrame::AVFrame()
{
	w = av_frame_alloc();
	if (!w)
	{
		throw "FFmpeg::AVFrame::create 失败";
	}
}

void FFmpeg::AVFrame::Dispose()
{
	if (should_dispose())
	{
		cout << "AVFrame 释放" << endl;
		av_frame_free(&w);
	}
}

void FFmpeg::AVFrame::av_frame_get_buffer(int align)
{
	int ret = ::av_frame_get_buffer(w, align);
	if (ret < 0)
		throw ret;
}

void FFmpeg::AVFrame::make_writable()
{
	int ret = ::av_frame_make_writable(w);
	if (ret < 0)
		throw ret;
}

void FFmpeg::AVFrame::unref()
{
	::av_frame_unref(w);
}

void FFmpeg::AVFrame::copy_image_to_buffer(FFmpeg::ImageBuffer buffer)
{
	av_image_copy(buffer._pointers, buffer._linesizes,
		(const uint8_t **)(w->data), w->linesize,
		(FFmpeg::AVPixelFormat)w->format, w->width, w->height);
}
