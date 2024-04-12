#pragma once
#include<ffmpeg-wrapper/container/HysteresisBlockingPacketQueue.h>
#include<ffmpeg-wrapper/pipe/DecoderPipe.h>
#include<jccpp/Exception.h>
#include<jccpp/TaskCompletionSignal.h>
#include<jccpp/define.h>

namespace video
{
	/// <summary>
	///		内部包装了一个线程地解码管道。
	/// </summary>
	class ThreadDecoderPipe final :
		public IPacketConsumer,
		public IPipeFrameSource,
		public IDisposable,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	public:
		ThreadDecoderPipe(AVStreamInfoCollection stream);
		~ThreadDecoderPipe();
		void Dispose() override;

	private:
		shared_ptr<DecoderPipe> _decoder_pipe;
		HysteresisBlockingPacketQueue _packet_queue{};
		TaskCompletionSignal _decode_thread_exit{ true };
		std::atomic_bool _do_not_flush_consumer = false;

		void DecodeThreadFunc();

	public:
		/// <summary>
		///		将包送入队列后就会立即返回，队列满了才会受到阻塞。
		///		另一个线程会负责从队列中取出包进行解码。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(AVPacketWrapper *packet) override;

		/// <summary>
		///		冲洗解码器，但是不冲洗消费者。
		///		当需要把本解码器撤掉，换另一个解码器时就要调用本方法。这样既可以取出残留在解码器
		///		中的帧，又不会把下级的消费者也一起冲洗了。
		/// </summary>
		void FlushDecoderButNotFlushConsumers();

		#pragma region 通过 IAudioStreamInfoCollection 继承
	public:
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;

		AVSampleFormat SampleFormat() const override;
		void SetSampleFormat(AVSampleFormat value) override;

		int SampleRate() const override;
		void SetSampleRate(int value) override;

		AVChannelLayout ChannelLayout() const override;
		void SetChannelLayout(AVChannelLayout value) override;
		#pragma endregion

		#pragma region 通过 IVideoStreamInfoCollection 继承
	public:
		int Width() const override;
		void SetWidth(int value) override;

		int Height() const override;
		void SetHeight(int value) override;

		AVPixelFormat PixelFormat() const override;
		void SetPixelFormat(AVPixelFormat value) override;

		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion

		List<shared_ptr<IFrameConsumer>> &ConsumerList() override
		{
			return _decoder_pipe->ConsumerList();
		}
	};
}