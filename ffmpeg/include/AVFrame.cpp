#include "AVFrame.h"
#include<AVUtil.h>
#include<ImageBuffer.h>

void FFmpeg::AVFrame::Dispose()
{
	if (should_dispose())
	{
		cout << "AVFrame 释放" << endl;
		av_frame_free(&_pWrapedObj);
	}
}

FFmpeg::AVFrame FFmpeg::AVFrame::create()
{
	FFmpeg::AVFrame frame;
	frame._pWrapedObj = av_frame_alloc();
	if (!frame._pWrapedObj)
	{
		throw "FFmpeg::AVFrame::create 失败";
	}

	return frame;
}

void FFmpeg::AVFrame::av_frame_get_buffer(int align)
{
	int ret = ::av_frame_get_buffer(_pWrapedObj, align);
	if (ret < 0)
		throw ret;
}

void FFmpeg::AVFrame::make_writable()
{
	int ret = ::av_frame_make_writable(_pWrapedObj);
	if (ret < 0)
		throw ret;
}

void FFmpeg::AVFrame::unref()
{
	::av_frame_unref(_pWrapedObj);
}

void FFmpeg::AVFrame::copy_image_to_arr(FFmpeg::ImageBuffer buffer)
{
	av_image_copy(buffer._pointers, buffer._linesizes,
		(const uint8_t **)(_pWrapedObj->data), _pWrapedObj->linesize,
		(FFmpeg::AVPixelFormat)_pWrapedObj->format, _pWrapedObj->width, _pWrapedObj->height);
}
