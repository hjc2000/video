#include"IVideoFrameInfoCollection.h"

using namespace video;

IVideoFrameInfoCollection &video::IVideoFrameInfoCollection::operator=(IVideoFrameInfoCollection const &another)
{
	SetWidth(another.Width());
	SetHeight(another.Height());
	SetPixelFormat(another.PixelFormat());
	return *this;
}

bool operator==(video::IVideoFrameInfoCollection const &left, video::IVideoFrameInfoCollection const &right)
{
	return left.Width() == right.Width() &&
		left.Height() == right.Height() &&
		left.PixelFormat() == right.PixelFormat();
}
