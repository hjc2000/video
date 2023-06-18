#pragma once
#include <Wraper.h>
#include <AVUtil.h>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
}

namespace FFmpeg
{
	class AVPacket : public Wraper<::AVPacket>
	{
#pragma region 生命周期
	public:
		AVPacket()
		{
			m_pWrapedObj = &m_packet;
		}
		AVPacket(::AVPacket *pAVPacket)
		{
			m_pWrapedObj = pAVPacket;
		}
		AVPacket(::AVPacket &ref_AVPacket)
		{
			m_pWrapedObj = &ref_AVPacket;
		}
		~AVPacket()
		{
			unref();
		}
#pragma endregion

#pragma region 私有字段
	private:
		::AVPacket m_packet;
#pragma endregion

#pragma region ffmpeg 中针对 AVPacket 的 API 的封装
	public:
		/**
		 * @brief 析构函数中会自动调用此方法
		 *
		 */
		void unref(void)
		{
			::av_packet_unref(m_pWrapedObj);
		}
#pragma endregion
#pragma region 扩展方法
		int64_t get_pts()
		{
			return m_packet.pts;
		}

		int64_t get_dts()
		{
			return m_packet.dts;
		}

#pragma endregion
	};
}