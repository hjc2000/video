#include<ffmpeg-wrapper/AVCodecExtention.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>

using namespace video;

AVStreamWrapper::AVStreamWrapper()
{

}

AVStreamWrapper::AVStreamWrapper(AVStream *p)
{
	_wrapped_obj = p;
}

AVStreamWrapper::AVStreamWrapper(AVStreamWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
}

AVStreamWrapper &AVStreamWrapper::operator=(AVStreamWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
	return *this;
}

AVCodec const *video::AVStreamWrapper::Codec()
{
	return AVCodecExtention::find_decoder_by_id(_wrapped_obj->codecpar->codec_id);
}

AVCodecParameters &AVStreamWrapper::CodecParams() const
{
	return *_wrapped_obj->codecpar;
}

void video::AVStreamWrapper::SetCodecParams(AVCodecParameters const &params)
{
	*_wrapped_obj->codecpar = params;
}

int video::AVStreamWrapper::SetCodecParams(shared_ptr<AVCodecContextWrapper> codec_ctx)
{
	// 设置时间基
	_wrapped_obj->time_base = (*codec_ctx)->time_base;
	_wrapped_obj->avg_frame_rate = codec_ctx->FrameRate();

	// 将码器的参数设置到本流
	return avcodec_parameters_from_context(_wrapped_obj->codecpar, *codec_ctx);
}

int64_t AVStreamWrapper::Bitrate()
{
	return _wrapped_obj->codecpar->bit_rate;
}

AVMediaType AVStreamWrapper::MediaType()
{
	return _wrapped_obj->codecpar->codec_type;
}

int AVStreamWrapper::Index()
{
	return _wrapped_obj->index;
}

#pragma region IAudioFrameInfoCollection
AVSampleFormat video::AVStreamWrapper::sample_format()
{
	return AVSampleFormat(_wrapped_obj->codecpar->format);
}

void video::AVStreamWrapper::set_sample_format(AVSampleFormat value)
{
	_wrapped_obj->codecpar->format = value;
}

AVChannelLayout video::AVStreamWrapper::ch_layout()
{
	return _wrapped_obj->codecpar->ch_layout;
}

void video::AVStreamWrapper::set_ch_layout(AVChannelLayout value)
{
	_wrapped_obj->codecpar->ch_layout = value;
}

int video::AVStreamWrapper::sample_rate() const
{
	return _wrapped_obj->codecpar->sample_rate;
}

void video::AVStreamWrapper::set_sample_rate(int value)
{
	_wrapped_obj->codecpar->sample_rate = value;
}
#pragma endregion

#pragma region IVideoStreamInfoCollection
int video::AVStreamWrapper::width()
{
	return _wrapped_obj->codecpar->width;
}
void video::AVStreamWrapper::set_width(int value)
{
	_wrapped_obj->codecpar->width = value;
}

int video::AVStreamWrapper::height()
{
	return _wrapped_obj->codecpar->height;
}
void video::AVStreamWrapper::set_height(int value)
{
	_wrapped_obj->codecpar->height = value;
}

AVPixelFormat video::AVStreamWrapper::pixel_format()
{
	return (AVPixelFormat)_wrapped_obj->codecpar->format;
}
void video::AVStreamWrapper::set_pixel_format(AVPixelFormat value)
{
	_wrapped_obj->codecpar->format = value;
}

AVRational video::AVStreamWrapper::TimeBase() const
{
	return _wrapped_obj->time_base;
}
void video::AVStreamWrapper::set_time_base(AVRational value)
{
	_wrapped_obj->time_base = value;
}

AVRational video::AVStreamWrapper::FrameRate() const
{
	return _wrapped_obj->avg_frame_rate;
}

void video::AVStreamWrapper::SetFrameRate(AVRational value)
{
	_wrapped_obj->avg_frame_rate = value;
}
#pragma endregion

