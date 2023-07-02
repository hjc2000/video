#pragma once
#include <Wraper.h>
#include <AVUtil.h>
#include <AVPacket.h>
#include<AVDictionary.h>
#include<AVStream.h>
#include<AVError.h>

namespace FFmpeg
{
	class AVFormatContext : public Wraper<::AVFormatContext>
	{
		#pragma region 生命周期
	public:
		AVFormatContext() {}
		AVFormatContext(::AVFormatContext *p) :Wraper(p) {}
		AVFormatContext(::AVFormatContext &ref) :Wraper(ref) {}

		~AVFormatContext()
		{
			if (should_dispose())
			{
				cout << "AVFormatContext 释放" << endl;
				if (_mode == Mode::output)
				{
					if (!(_pWrapedObj->oformat->flags & AVFMT_NOFILE))
						avio_closep(&_pWrapedObj->pb);
				}
				// 不管是输入还是输出，尽管调用释放资源的函数，反正不会发生异常
				// ffmpeg 内部有防御措施，不会对 nullptr 执行释放资源的操作
				::avformat_close_input(&_pWrapedObj);
				::avformat_free_context(_pWrapedObj);
			}
		}
		#pragma endregion

		#pragma region 私有字段
	private:
		/**
		 * @brief 用来标识 AVFormatContext 当前是哪一种模式的枚举类型
		*/
		enum Mode
		{
			unknow,
			input,
			output,
		};

		/**
		 * @brief 本对象当前是哪一种模式
		*/
		Mode _mode = Mode::unknow;
		#pragma endregion

		#pragma region 初始化函数
	public:
		/// <summary>
		/// 打开指定 url 作为输入。作为输入后无法再将此对象变成输出
		/// </summary>
		/// <param name="url"></param>
		/// <param name="fmt"></param>
		/// <param name="options"></param>
		inline void open_input(const char *url, const ::AVInputFormat *fmt = nullptr, ::AVDictionary **options = nullptr)
		{
			if (_mode != Mode::unknow)
				throw Exception("非法操作，不要重复初始化");
			_mode = Mode::input;
			int ret = ::avformat_open_input(&_pWrapedObj, url, fmt, options);
			if (ret < 0)
				throw Exception("AVFormatContext::open_input", ret);
		}

		/// <summary>
		/// 创建输出格式上下文。作为输出后无法再将此对象变成输入
		/// </summary>
		/// <param name="filename"></param>
		void alloc_output_context2(const char *filename)
		{
			if (_mode != Mode::unknow)
				throw Exception("非法操作，不要重复初始化");
			_mode = Mode::output;
			int ret = ::avformat_alloc_output_context2(&_pWrapedObj, nullptr, nullptr, filename);
			if (ret < 0)
				throw Exception("alloc_output_context2", ret);
			// 如果没有打开 IO 则打开 IO
			if (!(_pWrapedObj->oformat->flags & AVFMT_NOFILE))
			{
				int ret = avio_open(&_pWrapedObj->pb, filename, AVIO_FLAG_WRITE);
				if (ret < 0)
					throw Exception("AVFormatContext::alloc_output_context2", ret);
			}
		}
		#pragma endregion

		#pragma region 包装方法
	public:
		/**
		 * @brief	以官方格式打印流信息
		 * @param index	想要打印信息的流的索引号
		 * @param url	自定义的 url，比如这是一个输出文件，那么 url 就是你定义的输出文件路径。
		 *				你随便输都没关系，这里的 url 的用途只有被显示
		 * @param is_output 是否是输出文件。你也可以随便定，这个参数的用途只是用来被显示
		*/
		void dump_format(int index, const char *url, int is_output)
		{
			::av_dump_format(_pWrapedObj, index, url, is_output);
		}

		/// @brief 通过读几个包来检测流信息。此操作不会导致读取进度向前推移，
		/// 被读取的包会放在缓冲区下次读取的时候接着用
		/// @param options 
		inline void find_stream_info(::AVDictionary **options = nullptr)
		{
			int ret = ::avformat_find_stream_info(_pWrapedObj, options);
			if (ret < 0)
				throw Exception("AVFormatContext::find_stream_info", ret);
		}

		/// <summary>
		/// 找出最好的流。如果找不到指定的流，会抛出异常
		/// </summary>
		/// <param name="type"></param>
		/// <returns>返回找到的流</returns>
		FFmpeg::AVStream find_best_stream(AVMediaType type)
		{
			int ret = ::av_find_best_stream(_pWrapedObj, type, -1, -1, nullptr, 0);
			if (ret < 0)
				throw Exception("AVFormatContext::find_best_stream(AVMediaType = " + FFmpeg::media_type_to_string(type) + ")", ret);
			else
				return _pWrapedObj->streams[ret];
		}

		/// <summary>
		///		读取一个包（未解码的音视频数据包）
		///		如果已经到达文件尾或发生错误，会返回 false，否则返回 true
		///		可以将本方法作为循环条件，在循环中反复读取包
		///		如果发生错误，本方法会打印信息
		/// </summary>
		/// <param name="packet"></param>
		/// <returns></returns>
		bool read_packet(FFmpeg::AVPacket packet)
		{
			int ret = ::av_read_frame(_pWrapedObj, packet);
			if (ret < 0)
			{
				cout << "AVFormatContext 的 read_packet 方法发生错误：" <<
					FFmpeg::error_code_to_str(ret) << endl;
				return false;
			}
			else
			{
				return true;
			}
		}

		/// <summary>
		///		只有输出格式才能使用这个函数
		/// </summary>
		/// <param name="pCodec"></param>
		/// <returns></returns>
		FFmpeg::AVStream create_new_stream(const ::AVCodec *pCodec = nullptr)
		{
			::AVStream *ps = avformat_new_stream(_pWrapedObj, pCodec);
			if (ps == nullptr)
			{
				cout << "create_new_stream 方法异常" << endl;
				throw Exception("创建流失败");
			}
			else
			{
				return ps;
			}
		}

		/// <summary>
		///		向格式写头部
		/// </summary>
		/// <param name="dic"></param>
		void write_header(FFmpeg::AVDictionary *dic = nullptr)
		{
			int ret;
			if (dic == nullptr)
				ret = ::avformat_write_header(_pWrapedObj, nullptr);
			else
				ret = ::avformat_write_header(_pWrapedObj, *dic);

			if (ret < 0)
			{
				cout << "write_header 方法异常：" << FFmpeg::error_code_to_str(ret) << endl;
				throw Exception("AVFormatContext::write_header", ret);
			}
		}

		/// <summary>
		///		以交织的方式向格式写入一个包。只有输出格式才能调用本方法。
		/// </summary>
		/// <param name="packet"></param>
		void interleaved_write_packet(FFmpeg::AVPacket packet)
		{
			int ret = ::av_interleaved_write_frame(_pWrapedObj, packet);
			if (ret < 0)
			{
				cout << "interleaved_write_packet 异常："
					<< FFmpeg::error_code_to_str(ret) << endl;
				throw Exception("AVFormatContext::receive_frame", ret);
			}
		}

		/// <summary>
		///		向格式写尾部
		/// </summary>
		void write_trailer()
		{
			int ret = ::av_write_trailer(_pWrapedObj);
			if (ret < 0)
			{
				cout << "write_trailer 异常：" <<
					FFmpeg::error_code_to_str(ret) << endl;
				throw Exception("AVFormatContext::receive_frame", ret);
			}
		}
		#pragma endregion

		#pragma region 扩展包装方法
	public:
		/// <summary>
		///		获取视频时长。要先调用 find_stream_info 方法分析流信息后才能调用此方法，否则得到
		///		的结果是错误的
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
		///		获取指定索引的流。流的索引号超出范围会抛出异常
		/// </summary>
		/// <param name="stream_index"></param>
		/// <returns></returns>
		FFmpeg::AVStream get_stream(int stream_index)
		{
			// 强制转换为无符号类型就不用判断 stream_index >= 0 了
			if ((uint32_t)stream_index >= _pWrapedObj->nb_streams)
			{
				throw "流索引号超出范围";
			}
			return FFmpeg::AVStream{_pWrapedObj->streams[stream_index]};
		}
	};
	#pragma endregion
}