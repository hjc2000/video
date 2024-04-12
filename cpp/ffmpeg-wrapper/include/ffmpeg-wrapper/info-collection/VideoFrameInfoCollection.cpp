#include<ffmpeg-wrapper/info-collection/VideoFrameInfoCollection.h>

int video::VideoFrameInfoCollection::width() const
{
	return _width;
}

void video::VideoFrameInfoCollection::set_width(int value)
{
	_width = value;
}

int video::VideoFrameInfoCollection::height() const
{
	return _height;
}

void video::VideoFrameInfoCollection::set_height(int value)
{
	_height = value;
}

AVPixelFormat video::VideoFrameInfoCollection::pixel_format() const
{
	return _pixel_format;
}

void video::VideoFrameInfoCollection::set_pixel_format(AVPixelFormat value)
{
	_pixel_format = value;
}
