#include "AVFrame.h"
#include<AVUtil.h>

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

void FFmpeg::AVFrame::copy_image_to_arr(uint8_t *dst_data[4], int dst_linesizes[4])
{
	av_image_copy(dst_data, dst_linesizes,
		(const uint8_t **)(_pWrapedObj->data), _pWrapedObj->linesize,
		(FFmpeg::AVPixelFormat)_pWrapedObj->format, _pWrapedObj->width, _pWrapedObj->height);
}
