#include "DecoderPipe.h"

#pragma region IAudioStreamInfoCollection, IVideoStreamInfoCollection
AVRational video::DecoderPipe::time_base()
{
	return _stream_infos._timebase;
}

void video::DecoderPipe::set_time_base(AVRational value)
{
	_stream_infos._timebase = value;
}

AVSampleFormat video::DecoderPipe::sample_format()
{
	return _stream_infos._sample_format;
}

void video::DecoderPipe::set_sample_format(AVSampleFormat value)
{
	_stream_infos._sample_format = value;
}

int video::DecoderPipe::sample_rate()
{
	return _stream_infos._sample_rate;
}

void video::DecoderPipe::set_sample_rate(int value)
{
	_stream_infos._sample_rate = value;
}

AVChannelLayout video::DecoderPipe::ch_layout()
{
	return _stream_infos._ch_layout;
}

void video::DecoderPipe::set_ch_layout(AVChannelLayout value)
{
	_stream_infos._ch_layout = value;
}

int video::DecoderPipe::width()
{
	return _stream_infos._width;
}

void video::DecoderPipe::set_width(int value)
{
	_stream_infos._width = value;
}

int video::DecoderPipe::height()
{
	return _stream_infos._height;
}

void video::DecoderPipe::set_height(int value)
{
	_stream_infos._height = value;
}

AVPixelFormat video::DecoderPipe::pixel_format()
{
	return _stream_infos._pixel_format;
}

void video::DecoderPipe::set_pixel_format(AVPixelFormat value)
{
	_stream_infos._pixel_format = value;
}

AVRational video::DecoderPipe::frame_rate()
{
	return _stream_infos._frame_rate;
}

void video::DecoderPipe::set_frame_rate(AVRational value)
{
	_stream_infos._frame_rate = value;
}
#pragma endregion

