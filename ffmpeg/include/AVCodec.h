#pragma once
#include<include_ffmpeg.h>
#include <Wraper.h>

namespace FFmpeg
{
	typedef ::AVCodecID AVCodecID;

	class AVCodec :public Wraper<::AVCodec>
	{
	public:// 构造函数、析构函数
		AVCodec() {}
		AVCodec(const ::AVCodec *p) :Wraper(p) {}
		AVCodec(::AVCodec &ref) :Wraper(ref) {}

	public:// 工厂函数
		/// <summary>
		/// 通过编码器的名字查找编码器。找到则返回编码器，失败会抛出异常
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		static FFmpeg::AVCodec find_encoder_by_name(const char *name);

		/// <summary>
		/// 通过编码器的 ID 查找编码器。找到则返回编码器，失败会抛出异常
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		static FFmpeg::AVCodec find_decoder_by_id(AVCodecID id);

	public:
		void operator=(const FFmpeg::AVCodec &ref);
	};
}
