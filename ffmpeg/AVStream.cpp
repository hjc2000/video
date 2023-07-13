#pragma once
#include<include_ffmpeg.h>
#include<AVStream.h>
#include<AVCodec.h>
#include<AVError.h>

void FFmpeg::AVStream::copy_parameters_to(FFmpeg::AVStream dst_stream)
{
	int result = ::avcodec_parameters_copy(dst_stream()->codecpar, w->codecpar);
	if (result < 0)
	{
		throw Exception("AVStream::copy_parameters_to", result);
	}
}

FFmpeg::AVCodec FFmpeg::AVStream::get_stream_codec()
{
	return FFmpeg::AVCodec::find_decoder_by_id(w->codecpar->codec_id);
}
