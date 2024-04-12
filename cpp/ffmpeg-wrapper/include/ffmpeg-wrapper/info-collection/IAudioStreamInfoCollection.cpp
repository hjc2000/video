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

bool operator==(video::IAudioStreamInfoCollection const &left, video::IAudioStreamInfoCollection const &right)
{
	return left.TimeBase() == right.TimeBase() &&
		left.SampleFormat() == right.SampleFormat() &&
		left.SampleRate() == right.SampleRate() &&
		left.ChannelLayout() == right.ChannelLayout();
}
