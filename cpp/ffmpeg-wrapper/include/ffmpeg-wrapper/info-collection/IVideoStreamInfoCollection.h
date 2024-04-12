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
			set_time_base(value.TimeBase());
			SetFrameRate(value.FrameRate());
			return *this;
		}

		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value)
		{
			*this = (IVideoStreamInfoCollection &)value;
			return *this;
		}

	public:
		virtual AVRational TimeBase() = 0;
		virtual void set_time_base(AVRational value) = 0;

		virtual AVRational FrameRate() const = 0;
		virtual void SetFrameRate(AVRational value) = 0;

	public:
		uint32_t FrameIntervalInMilliseconds()
		{
			return 1000 * FrameRate().den / FrameRate().num;
		}

		double db_frame_rate()
		{
			return av_q2d(FrameRate());
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
