#include "sdl2-wrapper/SDL_AudioSpecWrapper.h"
#include<ffmpeg-wrapper/AVChannelLayoutExtension.h>
#include<jccpp/Exception.h>

video::SDL_AudioSpecWrapper::SDL_AudioSpecWrapper(IAudioFrameInfoCollection const &infos)
{
	IAudioFrameInfoCollection::operator=(infos);
}

video::SDL_AudioSpecWrapper::SDL_AudioSpecWrapper(IAudioStreamInfoCollection const &infos)
{
	IAudioStreamInfoCollection::operator=(infos);
}

video::SDL_AudioSpecWrapper::SDL_AudioSpecWrapper(SDL_AudioSpecWrapper &another)
{
	*this = another;
}

void video::SDL_AudioSpecWrapper::operator=(SDL_AudioSpecWrapper const &another)
{
	_spec = another._spec;
}

Json video::SDL_AudioSpecWrapper::ToJson()
{
	return Json{
		{"freq",_wrapped_obj->freq},
		{
			"format",
			{
				{"SDL_AudioFormat",_wrapped_obj->format},
				{"AVSampleFormat",sample_format_string()},
			},
		},
		{"channels",(int)_wrapped_obj->channels},
		{
			// 这个值表示：什么值表示静音。对于 s8，静音值是 0，对于 u8，静音值是 128.
			"silence",
			(int)_wrapped_obj->silence
		},
		{"samples",_wrapped_obj->samples},
		{"size",_wrapped_obj->size},
	};

	// 这个值是 SDL 内部使用的，用户不能使用，作用是内存对齐
	//sb << "padding = " << _wrapped_obj->padding << endl;
}

AVRational video::SDL_AudioSpecWrapper::TimeBase() const
{
	return AVRational{ 1,SampleRate() };
}

void video::SDL_AudioSpecWrapper::SetTimeBase(AVRational value)
{
	throw std::runtime_error{ "不支持的操作" };
}

AVSampleFormat video::SDL_AudioSpecWrapper::SampleFormat() const
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

void video::SDL_AudioSpecWrapper::SetSampleFormat(AVSampleFormat value)
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

int video::SDL_AudioSpecWrapper::SampleRate() const
{
	return _wrapped_obj->freq;
}

void video::SDL_AudioSpecWrapper::SetSampleRate(int value)
{
	_wrapped_obj->freq = value;
}

AVChannelLayout video::SDL_AudioSpecWrapper::ChannelLayout() const
{
	return AVChannelLayoutExtension::GetDefaultChannelLayout(_wrapped_obj->channels);
}

void video::SDL_AudioSpecWrapper::SetChannelLayout(AVChannelLayout value)
{
	_wrapped_obj->channels = value.nb_channels;
}

int video::SDL_AudioSpecWrapper::SampleCount() const
{
	return _wrapped_obj->samples;
}

void video::SDL_AudioSpecWrapper::SetSampleCount(int value)
{
	_wrapped_obj->samples = value;
}
