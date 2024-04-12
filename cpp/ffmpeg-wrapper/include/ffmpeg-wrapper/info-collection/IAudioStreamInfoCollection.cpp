#include"IAudioStreamInfoCollection.h"

using namespace video;

IAudioStreamInfoCollection &video::IAudioStreamInfoCollection::operator=(IAudioStreamInfoCollection const &value)
{
	SetTimeBase(value.TimeBase());
	SetSampleFormat(value.SampleFormat());
	SetSampleRate(value.SampleRate());
	SetChannelLayout(value.ChannelLayout());
	return *this;
}
