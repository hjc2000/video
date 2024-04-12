#pragma once
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>

namespace video
{
	class VideoStreamInfoCollection :public IVideoStreamInfoCollection
	{
	public:
		VideoStreamInfoCollection() {}

		VideoStreamInfoCollection(shared_ptr<IVideoStreamInfoCollection> value)
		{
			*this = value;
		}

		VideoStreamInfoCollection(IVideoStreamInfoCollection const &infos)
		{
			IVideoStreamInfoCollection::operator=(infos);
		}

	public:
		VideoStreamInfoCollection &operator=(shared_ptr<IVideoStreamInfoCollection> value)
		{
			*this = *value;
			return *this;
		}

		VideoStreamInfoCollection &operator=(IVideoStreamInfoCollection &value)
		{
			IVideoStreamInfoCollection::operator=(value);
			return *this;
		}
		VideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value)
		{
			IVideoStreamInfoCollection::operator=(value);
			return *this;
		}

	public:
		int _width = 0;
		int width() override
		{
			return _width;
		}
		void set_width(int value) override
		{
			_width = value;
		}

		int _height = 0;
		int height() override
		{
			return _height;
		}
		void set_height(int value) override
		{
			_height = value;
		}

		AVPixelFormat _pixel_format = AVPixelFormat{};
		AVPixelFormat pixel_format() override
		{
			return _pixel_format;
		}
		void set_pixel_format(AVPixelFormat value) override
		{
			_pixel_format = value;
		}

		AVRational _frame_rate = AVRational{};
		AVRational FrameRate() const override
		{
			return _frame_rate;
		}
		void set_frame_rate(AVRational value) override
		{
			_frame_rate = value;
		}

		AVRational _time_base = AVRational{};
		virtual AVRational time_base() override
		{
			return _time_base;
		}
		virtual void set_time_base(AVRational value) override
		{
			_time_base = value;
		}
	};
}