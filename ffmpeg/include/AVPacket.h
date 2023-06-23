#pragma once
#include<include_ffmpeg.h>
#include <Wraper.h>
#include <AVUtil.h>

namespace FFmpeg
{
	class AVPacket : public Wraper<::AVPacket>
	{
	public:
		AVPacket()
		{
			_pWrapedObj = new ::AVPacket{};
		}
		AVPacket(::AVPacket* p)
		{
			_pWrapedObj = p;
		}
		AVPacket(::AVPacket& ref)
		{
			_pWrapedObj = &ref;
		}
		~AVPacket()
		{
			if (should_dispose())
			{
				::av_packet_unref(_pWrapedObj);
				delete _pWrapedObj;
			}
		}

	public:
		int64_t& pts()
		{
			return _pWrapedObj->pts;
		}

		int64_t& dts()
		{
			return _pWrapedObj->dts;
		}
	};
}