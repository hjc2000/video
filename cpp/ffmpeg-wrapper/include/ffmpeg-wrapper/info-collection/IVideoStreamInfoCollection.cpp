#include"IVideoStreamInfoCollection.h"

using namespace video;

bool operator==(video::IVideoStreamInfoCollection const &left, video::IVideoStreamInfoCollection const &right)
{
	return (video::IVideoFrameInfoCollection &)left == (video::IVideoFrameInfoCollection &)right &&
		left.TimeBase() == right.TimeBase() &&
		left.FrameRate() == right.FrameRate();
}
