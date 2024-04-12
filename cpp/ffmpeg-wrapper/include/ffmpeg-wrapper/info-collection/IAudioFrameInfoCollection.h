#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>

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
			SetSampleCount(value.SampleCount());
			return *this;
		}

	public:
		virtual int SampleCount() const = 0;
		virtual void SetSampleCount(int value) = 0;
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
		left.SampleCount() == right.SampleCount();
}
