#include"AVStreamInfoCollection.h"

using namespace video;

AVRational video::AVStreamInfoCollection::time_base()
{
	return _timebase;
}

void video::AVStreamInfoCollection::set_time_base(AVRational value)
{
	_timebase = value;
}

AVSampleFormat video::AVStreamInfoCollection::sample_format()
{
	return _sample_format;
}

void video::AVStreamInfoCollection::set_sample_format(AVSampleFormat value)
{
	_sample_format = value;
}

int video::AVStreamInfoCollection::sample_rate()
{
	return _sample_rate;
}

void video::AVStreamInfoCollection::set_sample_rate(int value)
{
	_sample_rate = value;
}

AVChannelLayout video::AVStreamInfoCollection::ch_layout()
{
	return _ch_layout;
}

void video::AVStreamInfoCollection::set_ch_layout(AVChannelLayout value)
{
	_ch_layout = value;
}

int video::AVStreamInfoCollection::width()
{
	return _width;
}

void video::AVStreamInfoCollection::set_width(int value)
{
	_width = value;
}

int video::AVStreamInfoCollection::height()
{
	return _height;
}

void video::AVStreamInfoCollection::set_height(int value)
{
	_height = value;
}

AVPixelFormat video::AVStreamInfoCollection::pixel_format()
{
	return _pixel_format;
}

void video::AVStreamInfoCollection::set_pixel_format(AVPixelFormat value)
{
	_pixel_format = value;
}

AVRational video::AVStreamInfoCollection::frame_rate()
{
	return _frame_rate;
}

void video::AVStreamInfoCollection::set_frame_rate(AVRational value)
{
	_frame_rate = value;
}
