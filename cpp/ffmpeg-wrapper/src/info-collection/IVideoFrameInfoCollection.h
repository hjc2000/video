#pragma once
#include<include_ffmpeg.h>

namespace video
{
	class IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoFrameInfoCollection() {}

	public:
		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection &another)
		{
			set_width(another.width());
			set_height(another.height());
			set_pixel_format(another.pixel_format());
			return *this;
		}

		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection const &another)
		{
			*this = (IVideoFrameInfoCollection &)another;
			return *this;
		}

	public:
		virtual int width() = 0;
		virtual void set_width(int value) = 0;

		virtual int height() = 0;
		virtual void set_height(int value) = 0;

		virtual AVPixelFormat pixel_format() = 0;
		virtual void set_pixel_format(AVPixelFormat value) = 0;
	};
}

/**
 * @brief 比较两个 IVideoFrameInfoCollection 是否相等。
 * @param left
 * @param right
 * @return
*/
inline bool operator==(video::IVideoFrameInfoCollection &left, video::IVideoFrameInfoCollection &right)
{
	return left.width() == right.width() &&
		left.height() == right.height() &&
		left.pixel_format() == right.pixel_format();
}

inline bool operator==(video::IVideoFrameInfoCollection const &left, video::IVideoFrameInfoCollection const &right)
{
	return (video::IVideoFrameInfoCollection &)left == (video::IVideoFrameInfoCollection &)right;
}
