#include "sdl2-wrapper/SDL_DefaultAudioDevice.h"

using namespace video;
using namespace std;

SDL_DefaultAudioDevice::SDL_DefaultAudioDevice()
{
	_desired_spec->callback = static_audio_callback;
	_desired_spec->userdata = this;

	int device_id = SDL_OpenAudio(_desired_spec, _abtained_spec);
	if (device_id < 0)
	{
		string error_str = "Could not open audio: " + string(SDL_GetError());
		throw jc::Exception(error_str);
	}
}

SDL_DefaultAudioDevice::~SDL_DefaultAudioDevice()
{
	Dispose();
	cout << __func__ << endl;
}

void SDL_DefaultAudioDevice::Dispose()
{
	std::lock_guard l(_not_private_methods_lock);
	if (_disposed) return;
	_disposed = true;

	SDL_CloseAudio();
}

void SDL_DefaultAudioDevice::static_audio_callback(void *userdata, uint8_t *stream, int len)
{
	SDL_DefaultAudioDevice *me = (SDL_DefaultAudioDevice *)userdata;
	me->_audio_callback(stream, len);
}

void SDL_DefaultAudioDevice::Pause(bool pause)
{
	std::lock_guard l(_not_private_methods_lock);
	SDL_PauseAudio(pause);
}

AVRational video::SDL_DefaultAudioDevice::TimeBase() const
{
	return _abtained_spec.TimeBase();
}

void video::SDL_DefaultAudioDevice::SetTimeBase(AVRational value)
{
	throw std::runtime_error{ "不支持的操作" };
}

AVSampleFormat video::SDL_DefaultAudioDevice::SampleFormat() const
{
	return _abtained_spec.SampleFormat();
}

void video::SDL_DefaultAudioDevice::SetSampleFormat(AVSampleFormat value)
{
	throw std::runtime_error{ "不支持的操作" };
}

int video::SDL_DefaultAudioDevice::SampleRate() const
{
	return _abtained_spec.SampleRate();
}

void video::SDL_DefaultAudioDevice::SetSampleRate(int value)
{
	throw std::runtime_error{ "不支持的操作" };
}

AVChannelLayout video::SDL_DefaultAudioDevice::ChannelLayout() const
{
	return _abtained_spec.ChannelLayout();
}

void video::SDL_DefaultAudioDevice::SetChannelLayout(AVChannelLayout value)
{
	throw std::runtime_error{ "不支持的操作" };
}

int video::SDL_DefaultAudioDevice::SampleCount() const
{
	return _abtained_spec.SampleCount();
}

void video::SDL_DefaultAudioDevice::SetSampleCount(int value)
{
	throw std::runtime_error{ "不支持的操作" };
}

Json video::SDL_DefaultAudioDevice::ToJson()
{
	return Json{
		{"_desired_spec", _desired_spec.ToJson()},
		{"_abtained_spec", _abtained_spec.ToJson()},
	};
}
