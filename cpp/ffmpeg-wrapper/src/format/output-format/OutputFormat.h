#pragma once
#include<AVPacketWrapper.h>
#include<AVProgramWrapper.h>
#include<AVStreamWrapper.h>
#include<IPacketConsumer.h>
#include<jccpp/container/List.h>
#include<jccpp/Wrapper.h>
#include<include_ffmpeg.h>
#include<mutex>
#include<thread>

namespace video
{
	/// <summary>
	///		输出格式的基类。
	///		本类的 _wrapped_obj 没有初始化，需要派生类继承后进行初始化。
	/// </summary>
	class OutputFormat :
		public Wrapper<AVFormatContext>,
		public IPacketConsumer
	{
	public:
		virtual ~OutputFormat() {}

	private:
		std::mutex _not_private_methods_lock;
		int _flush_times = 0;
		List<int> _flushed_stream_index_list;

		/**
		* @brief 向格式写尾部
		*/
		void WriteTrailer();

	public:
		/// <summary>
		///		封装的数据结束后会触发此回调。此回调会启动后台线程来执行，避免用户
		///		在回调中调用本对象的加了互斥锁的方法，造成死锁。
		/// </summary>
		std::function<void()> _on_all_stream_flushed_async;

		void DumpFormat(char const *url = "");

		/// <summary>
		///		检查此输出格式是否需要设置全局头部。
		/// </summary>
		/// <returns></returns>
		bool NeedGlobalHeader();

		/// <summary>
		///		送入包。
		///		* 送入空指针进行冲洗。会对冲洗进行计数，只有当冲洗次数
		///		  等于流的数量的那一刻才会真正执行冲洗工作。注意，这里只是进行计数，
		///		  不会检查是是真的对每一路流进行冲洗了，还是只是对一路流多次冲洗。
		///		  所以，不要对同一路流进行多次冲洗。
		///		* 冲洗后，会自动调用 WriteTrailer 方法。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(AVPacketWrapper *packet) override;

		/**
		 * @brief 向格式写头部
		 * @param dic
		*/
		void WriteHeader(AVDictionary **dic = nullptr);

		/**
		 * @brief 创建一个新的流。只有输出格式才能使用这个函数
		 * @param pCodec 码器
		 * @return 创建成功则返回 AVStreamWrapper 的共享指针。失败会抛出异常。
		*/
		AVStreamWrapper CreateNewStream();

		/**
		 * @brief 创建流，并用传进来的编码器来设置流的参数。设置的参数中包括时间基和帧率。对于音频流，帧率是无用的，
		 * 但是设置了也没什么不好的影响。
		 *
		 * @param codec_ctx
		 * @return 创建流成功则返回流
		 *
		 * @exception Exception 创建流失败或设置流参数失败会抛出异常
		*/
		AVStreamWrapper CreateNewStream(shared_ptr<AVCodecContextWrapper> codec_ctx);
	};
}