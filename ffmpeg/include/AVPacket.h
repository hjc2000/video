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
	public:
		AVPacket()
		{
			m_pWrapedObj = &m_packet;
		}
		AVPacket(::AVPacket* pAVPacket)
		{
			m_pWrapedObj = pAVPacket;
		}
		AVPacket(::AVPacket& ref_AVPacket)
		{
			m_pWrapedObj = &ref_AVPacket;
		}
		~AVPacket()
		{
			unref();
		}

	private:
		::AVPacket m_packet = ::AVPacket();

	public:
		/**
		 * @brief 析构函数中会自动调用此方法
		 *
		 */
		void unref(void)
		{
			::av_packet_unref(m_pWrapedObj);
		}
		int64_t& get_pts()
		{
			return m_packet.pts;
		}

		int64_t& get_dts()
		{
			return m_packet.dts;
		}
	};
}