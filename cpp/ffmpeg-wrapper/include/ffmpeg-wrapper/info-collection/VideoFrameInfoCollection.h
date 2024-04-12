#pragma once
#include<ffmpeg-wrapper/info-collection/IVideoFrameInfoCollection.h>

namespace video
{
	class VideoFrameInfoCollection :public IVideoFrameInfoCollection
	{
	public:
		VideoFrameInfoCollection() {}

		VideoFrameInfoCollection(IVideoFrameInfoCollection &another)
		{
			IVideoFrameInfoCollection::operator=(another);
		}

	public:
		VideoFrameInfoCollection &operator=(IVideoFrameInfoCollection &value)
		{
			IVideoFrameInfoCollection::operator=(value);
			return *this;
		}

	public:
		int Width() const override;
		void set_width(int value) override;

		int Height() const override;
		void set_height(int value) override;

		AVPixelFormat PixelFormat() const override;
		void set_pixel_format(AVPixelFormat value) override;

	public:
		int _width = 0;
		int _height = 0;
		AVPixelFormat _pixel_format{};
	};
}