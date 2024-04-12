#include"IAudioFrameInfoCollection.h"

using namespace video;

IAudioFrameInfoCollection &video::IAudioFrameInfoCollection::operator=(IAudioFrameInfoCollection const &value)
{
	IAudioStreamInfoCollection::operator=(value);
	SetSampleCount(value.SampleCount());
	return *this;
}

bool operator==(video::IAudioFrameInfoCollection const &left, video::IAudioFrameInfoCollection const &right)
{
	return left == right && left.SampleCount() == right.SampleCount();
}
