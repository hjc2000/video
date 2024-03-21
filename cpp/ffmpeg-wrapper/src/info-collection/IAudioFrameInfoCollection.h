#pragma once
#include<AVError.h>
#include<IAudioStreamInfoCollection.h>
#include<include_ffmpeg.h>

namespace video
{
	/// <summary>
	/// 继承此接口表示含有音频帧的信息
	/// </summary>
	class IAudioFrameInfoCollection : public IAudioStreamInfoCollection
	{
	public:
		virtual ~IAudioFrameInfoCollection() {}

	public:
		IAudioFrameInfoCollection &operator=(IAudioFrameInfoCollection &value)
		{
			IAudioStreamInfoCollection::operator=(value);
			set_nb_samples(value.nb_samples());
			return *this;
		}

	public:
		virtual int nb_samples() = 0;
		virtual void set_nb_samples(int value) = 0;
	};

}

/**
 * @brief 比较两个 IAudioFrameInfoCollection 是否相等。
 * @param left
 * @param right
 * @return
*/
inline bool operator==(video::IAudioFrameInfoCollection &left, video::IAudioFrameInfoCollection &right)
{
	return (video::IAudioStreamInfoCollection &)left == (video::IAudioStreamInfoCollection &)right &&
		left.nb_samples() == right.nb_samples();
}
