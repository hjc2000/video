#pragma once
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>

namespace video
{
	/// <summary>
	///		继承此接口表示含有音频帧的信息
	/// </summary>
	class IAudioFrameInfoCollection : public IAudioStreamInfoCollection
	{
	public:
		virtual ~IAudioFrameInfoCollection() {}
		IAudioFrameInfoCollection &operator=(IAudioFrameInfoCollection const &value);

		virtual int SampleCount() const = 0;
		virtual void SetSampleCount(int value) = 0;

		using IAudioStreamInfoCollection::operator==;
		bool operator==(IAudioFrameInfoCollection const &another) const;
	};

}
