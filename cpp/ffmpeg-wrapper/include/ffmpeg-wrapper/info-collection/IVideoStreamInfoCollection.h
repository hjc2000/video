#pragma once
#include<ffmpeg-wrapper/AVCompare.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/IVideoFrameInfoCollection.h>

namespace video
{
	class IVideoStreamInfoCollection :public IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoStreamInfoCollection() = default;

	public:
		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection &value)
		{
			IVideoFrameInfoCollection::operator=(value);
			SetTimeBase(value.TimeBase());
			SetFrameRate(value.FrameRate());
			return *this;
		}

		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value)
		{
			*this = (IVideoStreamInfoCollection &)value;
			return *this;
		}

	public:
		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;

		virtual AVRational FrameRate() const = 0;
		virtual void SetFrameRate(AVRational value) = 0;

	public:
		uint32_t FrameIntervalInMilliseconds() const
		{
			return 1000 * FrameRate().den / FrameRate().num;
		}
	};
}

/**
 * @brief 比较两个 IVideoStreamInfoCollection 是否相等。
 * @param left
 * @param right
 * @return
*/
inline bool operator==(video::IVideoStreamInfoCollection &left, video::IVideoStreamInfoCollection &right)
{
	return (video::IVideoFrameInfoCollection &)left == (video::IVideoFrameInfoCollection &)right &&
		left.TimeBase() == right.TimeBase() &&
		left.FrameRate() == right.FrameRate();
}

inline bool operator==(video::IVideoStreamInfoCollection const &left, video::IVideoStreamInfoCollection const &right)
{
	return (video::IVideoStreamInfoCollection &)left == (video::IVideoStreamInfoCollection &)right;
}
