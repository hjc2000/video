#pragma once
#include<include_ffmpeg.h>
#include<Wraper.h>
#include<AVCodec.h>

namespace FFmpeg
{
	class AVStream :public Wraper<::AVStream>
	{
	public:
		AVStream() {}
		AVStream(::AVStream *p)
		{
			_pWrapedObj = p;
		}
		AVStream(::AVStream &ref)
		{
			_pWrapedObj = &ref;
		}

		#pragma region 包装方法
	public:
		/// <summary>
		/// 将此流的参数复制到指定的流
		/// </summary>
		/// <param name="dst_stream"></param>
		void copy_parameters_to(FFmpeg::AVStream dst_stream)
		{
			int result = avcodec_parameters_copy(dst_stream()->codecpar, _pWrapedObj->codecpar);
			if (result < 0)
				throw result;
		}
		#pragma endregion

		#pragma region 扩展方法
		/// <summary>
		/// 获取此流对应的编解码器
		/// </summary>
		/// <returns></returns>
		FFmpeg::AVCodec get_stream_codec();
		#pragma endregion

	};
}