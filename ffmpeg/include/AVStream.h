#pragma once
#include<include_ffmpeg.h>
#include<Wraper.h>

namespace FFmpeg
{
	class AVCodec;

	class AVStream :public Wraper<::AVStream>
	{
		#pragma region 生命周期
	public:
		AVStream(::AVStream *p) :Wraper(p) {}
		AVStream(::AVStream &ref) :Wraper(ref) {}
		#pragma endregion

		#pragma region 包装方法
	public:
		/// <summary>
		/// 将此流的参数复制到指定的流
		/// </summary>
		/// <param name="dst_stream"></param>
		void copy_parameters_to(FFmpeg::AVStream dst_stream);
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