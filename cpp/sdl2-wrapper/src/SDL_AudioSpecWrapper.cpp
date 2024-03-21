#include "SDL_AudioSpecWrapper.h"
#include<AVChannelLayoutExtension.h>
#include<Exception.h>

AVRational video::SDL_AudioSpecWrapper::time_base()
{
	return AVRational{ 1,sample_rate() };
}

void video::SDL_AudioSpecWrapper::set_time_base(AVRational value)
{
	throw jc::NotSupportedException();
}

AVSampleFormat video::SDL_AudioSpecWrapper::sample_format()
{
	switch (_wrapped_obj->format)
	{
	case AUDIO_U8:
		{
			return AVSampleFormat::AV_SAMPLE_FMT_U8;
		}
	case AUDIO_S16SYS:
		{
			return AVSampleFormat::AV_SAMPLE_FMT_S16;
		}
	case AUDIO_S32SYS:
		{
			return AVSampleFormat::AV_SAMPLE_FMT_S32;
		}
	case AUDIO_F32SYS:
		{
			return AVSampleFormat::AV_SAMPLE_FMT_FLT;
		}
	default:
		{
			return AVSampleFormat::AV_SAMPLE_FMT_NONE;
		}
	}
}

void video::SDL_AudioSpecWrapper::set_sample_format(AVSampleFormat value)
{
	switch (value)
	{
	case AVSampleFormat::AV_SAMPLE_FMT_U8:
		{
			_wrapped_obj->format = AUDIO_U8;
			break;
		}
	case AVSampleFormat::AV_SAMPLE_FMT_S16:
		{
			_wrapped_obj->format = AUDIO_S16SYS;
			break;
		}
	case AVSampleFormat::AV_SAMPLE_FMT_S32:
		{
			_wrapped_obj->format = AUDIO_S32SYS;
			break;
		}
	case AVSampleFormat::AV_SAMPLE_FMT_FLT:
		{
			_wrapped_obj->format = AUDIO_F32SYS;
			break;
		}
	default:
		{
			_wrapped_obj->format = 0;
		}
	}
}

int video::SDL_AudioSpecWrapper::sample_rate()
{
	return _wrapped_obj->freq;
}

void video::SDL_AudioSpecWrapper::set_sample_rate(int value)
{
	_wrapped_obj->freq = value;
}

AVChannelLayout video::SDL_AudioSpecWrapper::ch_layout()
{
	return AVChannelLayoutExtension::get_default_ch_layout(_wrapped_obj->channels);
}

void video::SDL_AudioSpecWrapper::set_ch_layout(AVChannelLayout value)
{
	_wrapped_obj->channels = value.nb_channels;
}

int video::SDL_AudioSpecWrapper::nb_samples()
{
	return _wrapped_obj->samples;
}

void video::SDL_AudioSpecWrapper::set_nb_samples(int value)
{
	_wrapped_obj->samples = value;
}
