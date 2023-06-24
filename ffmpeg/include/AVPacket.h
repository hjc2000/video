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
				unref();
				delete _pWrapedObj;
			}
		}

	public:
		/// <summary>
		/// 解除此包对缓冲区的引用
		/// </summary>
		void unref()
		{
			::av_packet_unref(_pWrapedObj);
		}

		void av_packet_rescale_ts(AVRational tb_src, AVRational tb_dst)
		{
			// 使用这个方法可以取代以下函数
			//packet()->pts = av_rescale_q_rnd(packet()->pts, input_stream()->time_base, output_stream()->time_base, ::AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//packet()->dts = av_rescale_q_rnd(packet()->dts, input_stream()->time_base, output_stream()->time_base, ::AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//packet()->duration = av_rescale_q(packet()->duration, input_stream()->time_base, output_stream()->time_base);
			// 直接一个函数完成 3 个函数的工作
			::av_packet_rescale_ts(_pWrapedObj, tb_src, tb_dst);
		}
	};
}