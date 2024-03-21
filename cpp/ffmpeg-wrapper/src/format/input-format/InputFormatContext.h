#pragma once
#include<AVIOContextWrapper.h>
#include<AVProgramWrapper.h>
#include<IPacketSource.h>
#include<Wrapper.h>
#include<functional>
#include<include_ffmpeg.h>
#include<string>

namespace video
{
	class AVPacketWrapper;
	class AVStreamWrapper;
	class AVCodecContextWrapper;

	class InputFormatContext :
		public Wrapper<AVFormatContext>,
		public IPacketSource
	{
	public:
		InputFormatContext(std::string url);

		InputFormatContext(shared_ptr<AVIOContextWrapper> io_context);

		InputFormatContext(std::string url, const AVInputFormat *fmt, AVDictionary **options);

		~InputFormatContext();

	private:
		string _url;
		shared_ptr<AVIOContextWrapper> _io_context;

		/// <summary>
		///		通过读几个包来检测流信息。此操作不会导致读取进度向前推移
		/// </summary>
		/// <param name="options"></param>
		void FindStreamInfo(::AVDictionary **options = nullptr);

	public:
		/// <summary>
		///		以官方格式打印流信息。
		/// </summary>
		void DumpFormat();

		/// <summary>
		///		找出最好的流
		/// </summary>
		/// <param name="type"></param>
		/// <returns>
		///		返回 AVStreamWrapper 对象。AVStreamWrapper 是个包装类，包装类重载了 bool 转换运算符，
		///		可以判断是否为空。
		/// </returns>
		AVStreamWrapper FindBestStream(AVMediaType type);

		/// <summary>
		///		从封装中读取包。
		///		* 在实际读取前，会先调用 packet 的 unref 方法。
		///		* ffmpeg 原生的读取包不会将流的时间基赋值给 AVPacket 的 time_base 字段。
		///		  本函数进行扩展，读取的包的 time_base 字段会被赋值。
		/// </summary>
		/// <param name="packet">读取到的包会写入这里。</param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int ReadPacket(AVPacketWrapper &packet) override;

	public:
		/**
		 * @brief 获取视频时长。要先调用 FindStreamInfo 方法分析流信息后才能调用此方法，否则得到
		 * 的结果是错误的
		 *
		 * @return 返回结果是一个字符串，里面储存着格式化过的时间
		*/
		std::string get_duration_as_formatted_time_string();

		/**
		 * @brief 流的数量
		 * @return
		*/
		int StreamCount()
		{
			return _wrapped_obj->nb_streams;
		}

		/**
		 * @brief 获取指定索引的流。流的索引号超出范围会抛出异常
		 * @param stream_index
		 * @return
		*/
		AVStreamWrapper GetStream(int stream_index);
	};
}