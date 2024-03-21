#pragma once
#include<IVideoFrameInfoCollection.h>
#include<include_ffmpeg.h>

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
		double db_frame_rate()
		{
			return av_q2d(frame_rate());
		}

		template <typename T>
		T frame_interval_in_milliseconds()
		{
			static_assert(std::is_same<T, double>::value || std::is_same<T, uint32_t>::value,
						  "Unsupported type for frame_interval_in_milliseconds");

			return T();
		}
	};

	// 类外的模板特化
	template <>
	inline double IVideoStreamInfoCollection::frame_interval_in_milliseconds<double>()
	{
		return 1000.0 / db_frame_rate();
	}

	template <>
	inline uint32_t IVideoStreamInfoCollection::frame_interval_in_milliseconds<uint32_t>()
	{
		return 1000 / static_cast<uint32_t>(db_frame_rate());
	}
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
