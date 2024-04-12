#pragma once
#include<ffmpeg-wrapper/AVCompare.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/IVideoFrameInfoCollection.h>

namespace video
{
	class IVideoStreamInfoCollection :public IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoStreamInfoCollection() {}

	public:
		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection &value)
		{
			IVideoFrameInfoCollection::operator=(value);
			set_time_base(value.time_base());
			set_frame_rate(value.frame_rate());
			return *this;
		}

		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value)
		{
			*this = (IVideoStreamInfoCollection &)value;
			return *this;
		}

	public:
		virtual AVRational time_base() = 0;
		virtual void set_time_base(AVRational value) = 0;

		virtual AVRational frame_rate() = 0;
		virtual void set_frame_rate(AVRational value) = 0;

	public:// 接口扩展
		uint32_t FrameIntervalInMilliseconds()
		{
			return 1000 / static_cast<uint32_t>(db_frame_rate());
		}

		double db_frame_rate()
		{
			return av_q2d(frame_rate());
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
		left.time_base() == right.time_base() &&
		left.frame_rate() == right.frame_rate();
}

inline bool operator==(video::IVideoStreamInfoCollection const &left, video::IVideoStreamInfoCollection const &right)
{
	return (video::IVideoStreamInfoCollection &)left == (video::IVideoStreamInfoCollection &)right;
}
