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
		IVideoStreamInfoCollection &operator=(IVideoStreamInfoCollection const &value);

		virtual AVRational TimeBase() const = 0;
		virtual void SetTimeBase(AVRational value) = 0;

		virtual AVRational FrameRate() const = 0;
		virtual void SetFrameRate(AVRational value) = 0;

		uint32_t FrameIntervalInMilliseconds() const;
	};
}

/// <summary>
///		比较两个 IVideoStreamInfoCollection 是否相等。
/// </summary>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns></returns>
bool operator==(video::IVideoStreamInfoCollection const &left, video::IVideoStreamInfoCollection const &right);
