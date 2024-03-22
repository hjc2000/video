#include"AVStreamInfoCollection.h"

using namespace video;

AVStreamInfoCollection::AVStreamInfoCollection(AVStreamInfoCollection const &stream)
{
	*this = stream;
}

AVStreamInfoCollection::AVStreamInfoCollection(AVStreamWrapper const &stream)
{
	*this = stream;
}

AVStreamInfoCollection::~AVStreamInfoCollection()
{
	avcodec_parameters_free(&_codec_params);
}

AVStreamInfoCollection &AVStreamInfoCollection::operator=(AVStreamInfoCollection const &stream)
{
	*this = (AVStreamInfoCollection &)stream;
	return *this;
}

AVStreamInfoCollection &AVStreamInfoCollection::operator=(AVStreamInfoCollection &stream)
{
	_index = stream._index;
	_media_type = stream._media_type;
	_bitrate = stream._bitrate;
	CopyCodecParamFrom(stream._codec_params);
	_codec = stream._codec;

	_timebase = stream._timebase;
	_sample_format = stream._sample_format;
	_sample_rate = stream._sample_rate;
	_ch_layout = stream._ch_layout;
	_width = stream._width;
	_height = stream._height;
	_pixel_format = stream._pixel_format;
	_frame_rate = stream._frame_rate;

	return *this;
}

AVStreamInfoCollection &AVStreamInfoCollection::operator = (AVStreamWrapper const &stream)
{
	*this = (AVStreamWrapper &)stream;
	return *this;
}

AVStreamInfoCollection &AVStreamInfoCollection::operator=(AVStreamWrapper &stream)
{
	_index = stream.Index();
	_media_type = stream.MediaType();
	_bitrate = stream.Bitrate();
	CopyCodecParamFrom(stream->codecpar);
	_codec = stream.Codec();

	_timebase = stream.time_base();
	_sample_format = stream.sample_format();
	_sample_rate = stream.sample_rate();
	_ch_layout = stream.ch_layout();
	_width = stream.width();
	_height = stream.height();
	_pixel_format = stream.pixel_format();
	_frame_rate = stream.frame_rate();

	return *this;
}

void video::AVStreamInfoCollection::CopyCodecParamFrom(AVCodecParameters *src)
{
	// avcodec_parameters_copy 会先释放 dst，然后再将 src 的数据复制到 dst。
	avcodec_parameters_copy(_codec_params, src);
}

int video::AVStreamInfoCollection::Index()
{
	return _index;
}

void video::AVStreamInfoCollection::SetIndex(int value)
{
	_index = value;
}

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
