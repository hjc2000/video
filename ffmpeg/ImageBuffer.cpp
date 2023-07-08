#include "ImageBuffer.h"

FFmpeg::ImageBuffer::ImageBuffer(FFmpeg::AVCodecContext ctx, int align)
{
	_size = av_image_alloc(_pointers, _linesizes,
		ctx()->width, ctx()->height,
		ctx()->pix_fmt, 1);
}
