#include"IVideoStreamInfoCollection.h"

using namespace video;

IVideoStreamInfoCollection &video::IVideoStreamInfoCollection::operator=(IVideoStreamInfoCollection const &value)
{
	IVideoFrameInfoCollection::operator=(value);
	SetTimeBase(value.TimeBase());
	SetFrameRate(value.FrameRate());
	return *this;
}

uint32_t video::IVideoStreamInfoCollection::FrameIntervalInMilliseconds() const
{
	return 1000 * FrameRate().den / FrameRate().num;
}

bool operator==(video::IVideoStreamInfoCollection const &left, video::IVideoStreamInfoCollection const &right)
{
	return (video::IVideoFrameInfoCollection &)left == (video::IVideoFrameInfoCollection &)right &&
		left.TimeBase() == right.TimeBase() &&
		left.FrameRate() == right.FrameRate();
}
