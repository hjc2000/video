#pragma once
#include<include_ffmpeg.h>
#include<AVStream.h>
#include<AVCodec.h>

FFmpeg::AVCodec FFmpeg::AVStream::get_stream_codec()
{
	return FFmpeg::AVCodec::find_decoder_by_id(_pWrapedObj->codecpar->codec_id);
}
