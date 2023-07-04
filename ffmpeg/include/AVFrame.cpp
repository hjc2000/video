#include "AVFrame.h"

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
