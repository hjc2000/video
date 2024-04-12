#pragma once
#include<ffmpeg-wrapper/base_include.h>

namespace video
{
	class IVideoFrameInfoCollection
	{
	public:
		virtual ~IVideoFrameInfoCollection() {}
		IVideoFrameInfoCollection &operator=(IVideoFrameInfoCollection const &another);

	public:
		virtual int Width() const = 0;
		virtual void SetWidth(int value) = 0;

		virtual int Height() const = 0;
		virtual void SetHeight(int value) = 0;

		virtual AVPixelFormat PixelFormat() const = 0;
		virtual void SetPixelFormat(AVPixelFormat value) = 0;
	};
}

/// <summary>
///		比较两个 IVideoFrameInfoCollection 是否相等。
/// </summary>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns></returns>
bool operator==(video::IVideoFrameInfoCollection const &left, video::IVideoFrameInfoCollection const &right);
