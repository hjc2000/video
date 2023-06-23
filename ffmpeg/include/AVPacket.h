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
			_pWrapedObj = &m_packet;
		}
		AVPacket(::AVPacket* pAVPacket)
		{
			_pWrapedObj = pAVPacket;
		}
		AVPacket(::AVPacket& ref_AVPacket)
		{
			_pWrapedObj = &ref_AVPacket;
		}
		~AVPacket()
		{
			// 因为本类的 _pWrapedObj 指针指向的是类字段，所以不需要使用 should_dispose，直接释放
			// 资源
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
			::av_packet_unref(_pWrapedObj);
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