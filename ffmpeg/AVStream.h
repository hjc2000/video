#pragma once

#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVStream :public Wraper<::AVStream>
	{
	public:
		AVStream() {}
		AVStream(::AVStream* pStream)
		{
			_pWrapedObj = pStream;
		}
		AVStream(::AVStream& refStream)
		{
			_pWrapedObj = &refStream;
		}

	public:
		/// @brief 将此流的参数复制到指定的流
		/// @param dst_stream 
		void copy_parameters_to(FFmpeg::AVStream dst_stream)
		{
			int result = avcodec_parameters_copy(dst_stream()->codecpar, _pWrapedObj->codecpar);
			if (result < 0)
				throw result;
		}
	};
}