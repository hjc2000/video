#pragma once
#include <Wraper.h>
#include <AVUtil.h>
#include <AVPacket.h>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
}
#include <string>
#include <sstream>

namespace FFmpeg
{
	class AVFormatContext : public Wraper<::AVFormatContext>
	{
#pragma region 生命周期
	public:
		AVFormatContext() {}
		AVFormatContext(::AVFormatContext *pFormatContext)
		{
			m_pWrapedObj = pFormatContext;
		}
		AVFormatContext(::AVFormatContext &ref_FormatContext)
		{
			m_pWrapedObj = &ref_FormatContext;
		}
		~AVFormatContext()
		{
			// 不管是输入还是输出，尽管调用释放资源的函数，反正不会发生异常
			// ffmpeg 内部有防御措施，不会对 nullptr 执行释放资源的操作
			::avformat_close_input(&m_pWrapedObj);
			::avformat_free_context(m_pWrapedObj);
		}
#pragma endregion

#pragma region 公共字段

#pragma endregion

#pragma region ffmpeg 中针对 AVFormatContext 的操作函数
	private:
		bool m_is_input = false;
		bool m_is_output = false;

	public:
		inline void open_input(const char *url, const ::AVInputFormat *fmt = nullptr, ::AVDictionary **options = nullptr)
		{
			if (m_is_output)
				throw "该上下文已经是输出了，不允许作为输入打开";
			m_is_input = true;
			int result = ::avformat_open_input(&m_pWrapedObj, url, fmt, options);
			if (result < 0)
				throw result;
		}

		void alloc_output_context2(const char *filename)
		{
			if (m_is_input)
				throw "该上下文已经是输入了，不允许作为输出打开";
			m_is_output = true;
			int result = ::avformat_alloc_output_context2(&m_pWrapedObj, nullptr, nullptr, filename);
			if (result < 0)
				throw result;
		}

		void dump_format(int index, const char *url, int is_output)
		{
			::av_dump_format(m_pWrapedObj, index, url, is_output);
		}

		/**
		 * @brief 通过读几个包来检测流信息。此操作不会导致读取进度向前推移，被读取的包会放在
		 * 缓冲区下次读取的时候接着用
		 *
		 * @param options
		 */
		inline void find_stream_info(::AVDictionary **options = nullptr)
		{
			int result = ::avformat_find_stream_info(m_pWrapedObj, options);
			if (result < 0)
				throw result;
		}

		/**
		 * @brief 找出最好的流
		 *
		 * @param type
		 * @param wanted_stream_nb
		 * @param related_stream
		 * @param decoder_ret
		 * @param flags
		 * @return int
		 */
		int find_best_stream(AVMediaType type)
		{
			int result = ::av_find_best_stream(m_pWrapedObj, type, -1, -1, nullptr, 0);
			if (result < 0)
				throw result;
			else
				return result;
		}

		void read_frame(FFmpeg::AVPacket &ref_packet)
		{
			int result = ::av_read_frame(m_pWrapedObj, ref_packet);
			if (result < 0)
				throw result;
		}
#pragma endregion

#pragma region 扩展方法
	public:
		/**
		 * @brief 获取视频时长
		 *
		 * @return std::string 返回结果是一个字符串，里面储存着格式化过的时间。格式为
		 * 时:分:秒
		 */
		inline std::string get_duration_as_formatted_time_string()
		{
			std::stringstream sstream;
			// 获取视频总的秒数
			int64_t total_seconds = m_pWrapedObj->duration / AV_TIME_BASE;
			// 取出秒
			int second = total_seconds % 60;
			// 扣除秒位后总的分钟数
			uint64_t total_minutes = total_seconds / 60;
			// 取出分钟
			int minute = total_minutes % 60;
			// 扣除分钟后总的小时数
			uint64_t total_hours = total_minutes / 60;
			sstream << total_hours << "时" << minute << "分" << second << "秒" << std::endl;
			std::string re_value;
			sstream >> re_value;
			return re_value;
		}
#pragma endregion
	};
}