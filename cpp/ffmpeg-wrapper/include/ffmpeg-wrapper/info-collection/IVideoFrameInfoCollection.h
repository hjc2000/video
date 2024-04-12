#pragma once
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	class IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoFrameInfoCollection() {}

	public:
		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection &another)
		{
			set_width(another.Width());
			set_height(another.Height());
			set_pixel_format(another.PixelFormat());
			return *this;
		}

		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection const &another)
		{
			*this = (IVideoFrameInfoCollection &)another;
			return *this;
		}

	public:
		virtual int Width() const = 0;
		virtual void set_width(int value) = 0;

		virtual int Height() const = 0;
		virtual void set_height(int value) = 0;

		virtual AVPixelFormat PixelFormat() const = 0;
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
	return left.Width() == right.Width() &&
		left.Height() == right.Height() &&
		left.PixelFormat() == right.PixelFormat();
}

inline bool operator==(video::IVideoFrameInfoCollection const &left, video::IVideoFrameInfoCollection const &right)
{
	return (video::IVideoFrameInfoCollection &)left == (video::IVideoFrameInfoCollection &)right;
}
