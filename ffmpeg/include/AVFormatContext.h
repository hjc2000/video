#pragma once
#include <Wraper.h>
#include <AVUtil.h>
#include <AVPacket.h>
#include<AVDictionary.h>
#include<AVStream.h>
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
	public:// 生命周期
		AVFormatContext() {}
		AVFormatContext(::AVFormatContext* pFormatContext)
		{
			_pWrapedObj = pFormatContext;
		}
		AVFormatContext(::AVFormatContext& refFormatContext)
		{
			_pWrapedObj = &refFormatContext;
		}
		void DisposeWrapedObj() override
		{
			if (m_is_output)
			{
				if (!(_pWrapedObj->oformat->flags & AVFMT_NOFILE))
					avio_closep(&_pWrapedObj->pb);
			}
			// 不管是输入还是输出，尽管调用释放资源的函数，反正不会发生异常
			// ffmpeg 内部有防御措施，不会对 nullptr 执行释放资源的操作
			::avformat_close_input(&_pWrapedObj);
			::avformat_free_context(_pWrapedObj);
		}

	private:// 私有字段
		bool m_is_input = false;
		bool m_is_output = false;
		bool _copyed = false;

	public:
		/// @brief 打开指定 url 作为输入。作为输入后无法再将此对象变成输出
		/// @param url 
		/// @param fmt 
		/// @param options 
		inline void open_input(const char* url, const ::AVInputFormat* fmt = nullptr, ::AVDictionary** options = nullptr)
		{
			if (m_is_output)
				throw "该上下文已经是输出了，不允许作为输入打开";
			m_is_input = true;
			int result = ::avformat_open_input(&_pWrapedObj, url, fmt, options);
			if (result < 0)
				throw result;
		}

		/// @brief 创建输出格式上下文。作为输出后无法再将此对象变成输入
		/// @param filename 
		void alloc_output_context2(const char* filename)
		{
			if (m_is_input)
				throw "该上下文已经是输入了，不允许作为输出打开";
			m_is_output = true;
			int result = ::avformat_alloc_output_context2(&_pWrapedObj, nullptr, nullptr, filename);
			if (result < 0)
				throw result;
			// 如果没有打开 IO 则打开 IO
			if (!(_pWrapedObj->oformat->flags & AVFMT_NOFILE))
			{
				int result = avio_open(&_pWrapedObj->pb, filename, AVIO_FLAG_WRITE);
				if (result < 0)
					throw result;
			}
		}

		/// @brief 对 FFmpeg 内部提供的打印流信息的函数的封装。会以官方格式打印流信息
		/// @param index 想要打印信息的流的索引号
		/// @param url 自定义的 url，比如这是一个输出文件，那么 url 就是你定义的输出文件路径。
		/// 你随便输都没关系，这里的 url 的用途只有被显示
		/// @param is_output 是否是输出文件。你也可以随便定，同样的，这个参数的用途只是用来被显示
		void dump_format(int index, const char* url, int is_output)
		{
			::av_dump_format(_pWrapedObj, index, url, is_output);
		}

		/// @brief 通过读几个包来检测流信息。此操作不会导致读取进度向前推移，
		/// 被读取的包会放在缓冲区下次读取的时候接着用
		/// @param options 
		inline void find_stream_info(::AVDictionary** options = nullptr)
		{
			int result = ::avformat_find_stream_info(_pWrapedObj, options);
			if (result < 0)
				throw result;
		}

		/// <summary>
		/// 找出最好的流
		/// </summary>
		/// <param name="type"></param>
		/// <returns>返回找到的流</returns>
		FFmpeg::AVStream find_best_stream(AVMediaType type)
		{
			int result = ::av_find_best_stream(_pWrapedObj, type, -1, -1, nullptr, 0);
			if (result < 0)
				throw result;
			else
				return _pWrapedObj->streams[result];
		}

		/// <summary>
		/// 读取一个包（未解码的音视频数据包）
		/// - 如果已经到达文件末尾，再次调用本方法会抛出异常
		/// - 如果发生错误，也会抛出异常
		/// </summary>
		/// <returns>返回读取到的包</returns>
		FFmpeg::AVPacket read_frame()
		{
			FFmpeg::AVPacket packet;
			int result = ::av_read_frame(_pWrapedObj, packet);
			if (result < 0)
				throw result;
			else
				return packet;
		}

		FFmpeg::AVStream create_new_stream(const ::AVCodec* pCodec = nullptr)
		{
			::AVStream* ps = avformat_new_stream(_pWrapedObj, pCodec);
			if (ps == nullptr)
				throw "创建流失败";
			else
				return ps;
		}

		void write_header(FFmpeg::AVDictionary* dic = nullptr)
		{
			int result;
			if (dic == nullptr)
				result = ::avformat_write_header(_pWrapedObj, nullptr);
			else
				result = ::avformat_write_header(_pWrapedObj, *dic);

			if (result < 0)
				throw result;
		}

		void interleaved_write_frame(FFmpeg::AVPacket packet)
		{
			int ret = ::av_interleaved_write_frame(_pWrapedObj, packet);
			if (ret < 0)
				throw ret;
		}

		void write_trailer()
		{
			int ret = ::av_write_trailer(_pWrapedObj);
			if (ret < 0)
				throw ret;
		}

	public:
		/// <summary>
		/// 获取视频时长
		/// </summary>
		/// <returns>返回结果是一个字符串，里面储存着格式化过的时间</returns>
		inline std::string get_duration_as_formatted_time_string()
		{
			std::stringstream sstream;
			// 获取视频总的秒数
			int64_t total_seconds = _pWrapedObj->duration / AV_TIME_BASE;
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

		/// <summary>
		/// 获取指定索引的流。流的索引号超出范围会抛出异常
		/// </summary>
		/// <param name="stream_index"></param>
		/// <returns></returns>
		FFmpeg::AVStream get_stream(int stream_index)
		{
			// 将 stream_index 强制转换为 uint32_t，如果 stream_index 是负数，会
			// 造成上溢，如果不是负数，但是大于 nb_streams，同样会造成上溢
			if ((uint32_t)stream_index >= _pWrapedObj->nb_streams)
			{
				throw "流索引号超出范围";
			}
			return FFmpeg::AVStream{_pWrapedObj->streams[stream_index]};
		}
	};
}