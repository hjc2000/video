#pragma once
#include<DecoderPipe.h>
#include<FileStream.h>
#include<IDisposable.h>
#include<InfinitePacketPipe.h>
#include<InputFormatContext.h>
#include<PacketPump.h>
#include<ProgramDecoder.h>
#include<atomic>
#include<thread>

namespace video
{
	/// <summary>
	///		将输入格式拼接起来，进行解封装、解码。一个输入格式结束后会继续获取
	///		下一个输入格式。最终输出来自不同输入格式解封装、解码后的视频帧、音频帧。
	/// </summary>
	class JoinedInputFormatDemuxDecoder :
		public IDisposable
	{
	public:
		void Dispose() override
		{

		}

	private:


	public:
		/// <summary>
		///		当需要输入封装时就会触发此回调。
		///		回调函数返回 InputFormatContext 对象则视频流继续。
		///		回调函数返回空指针则结束视频流。
		/// </summary>
		std::function<shared_ptr<InputFormatContext>()> _get_format_callback;
	};

	/// <summary>
	///		无限长的解封装、解码器。会在一个输入封装读取完后重新获取下一个封装。
	///		* 第二个及以后的封装的流参数必须与第一个封装一样。
	/// </summary>
	class InfiniteBestStreamDemuxDecoder_old :
		public IDisposable,
		public IMultiStreamPipeFrameSource
	{
	public:
		/// <summary>
		///		构造函数中就会调用 get_format_callback 来获取第一个输入格式。
		/// </summary>
		/// <param name="get_format_callback">此回调函数返回空指针会结束流。</param>
		InfiniteBestStreamDemuxDecoder_old(
			std::function<shared_ptr<InputFormatContext>()> get_format_callback
		);

		~InfiniteBestStreamDemuxDecoder_old()
		{
			Dispose();
		}

		/// <summary>
		///		会向 Pump 线程发出退出消息后阻塞，等待 Pump 线程退出。
		/// </summary>
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_cancel_pump.Cancel();
			_pump_thread_has_exit.Wait();
		}

	private:
		std::atomic_bool _disposed = false;
		shared_ptr<InputFormatContext> _in_fmt_ctx;
		CancellationTokenSource _cancel_pump;
		shared_ptr<ProgramDecoder> _program_decoder;
		shared_ptr<InfinitePacketPipe> _infinite_packet_pipe{ new InfinitePacketPipe{} };
		TaskCompletionSignal _pump_thread_has_exit{ true };
		shared_ptr<FileStream> _fstream;
		std::function<shared_ptr<InputFormatContext>()> _get_format_callback;
		List<AVStreamInfoCollection> _stream_list;
		bool _pump_started = false;

		/// <summary>
		///		输入封装中的原始视频流索引。每次打开输入封装后都会更新。
		/// </summary>
		int _src_video_stream_index = -1;

		/// <summary>
		///		输入封装中的原始音频流索引。每次打开输入封装后都会更新。
		/// </summary>
		int _src_audio_stream_index = -1;

		/// <summary>
		///		打开输入
		/// </summary>
		/// <returns></returns>
		bool OpenInput();

	public:
		/// <summary>
		///		创建后台线程开始沿管道泵送数据。
		/// </summary>
		void StartPump();

		List<AVStreamInfoCollection> &StreamList() override;
		shared_ptr<IPipeFrameSource> GetPipeFrameSourceByStreamIndex(int stream_index) override;
	};
}