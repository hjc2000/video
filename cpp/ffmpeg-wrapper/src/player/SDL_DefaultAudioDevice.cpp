#include "SDL_DefaultAudioDevice.h"

AVRational video::SDL_DefaultAudioDevice::time_base()
{
    return _abtained_spec.time_base();
}

void video::SDL_DefaultAudioDevice::set_time_base(AVRational value)
{
}

AVSampleFormat video::SDL_DefaultAudioDevice::sample_format()
{
    return _abtained_spec.sample_format();
}

void video::SDL_DefaultAudioDevice::set_sample_format(AVSampleFormat value)
{
}

int video::SDL_DefaultAudioDevice::sample_rate()
{
    return _abtained_spec.sample_rate();
}

void video::SDL_DefaultAudioDevice::set_sample_rate(int value)
{
}

AVChannelLayout video::SDL_DefaultAudioDevice::ch_layout()
{
    return _abtained_spec.ch_layout();
}

void video::SDL_DefaultAudioDevice::set_ch_layout(AVChannelLayout value)
{
}

int video::SDL_DefaultAudioDevice::nb_samples()
{
    return _abtained_spec.nb_samples();
}

void video::SDL_DefaultAudioDevice::set_nb_samples(int value)
{
}
