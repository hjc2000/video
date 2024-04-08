#pragma once
#include<AVCodecContextWrapper.h>
#include<AVStreamInfoCollection.h>
#include<AVStreamWrapper.h>
#include<AVToString.h>
#include<ErrorCode.h>
#include<jccpp/IDisposable.h>
#include<IFrameConsumer.h>
#include<IPacketConsumer.h>
#include<jccpp/container/List.h>
#include<PipeFrameSource.h>
#include<jccpp/TaskCompletionSignal.h>
#include<atomic>
#include<vector>

namespace video
{
	/**
	 * @brief 送入包，会解码然后将帧送给 IFrameConsumer。
	*/
	class DecoderPipe :
		public IPacketConsumer,
		public PipeFrameSource,
		public IDisposable,
		public IAudioStreamInfoCollection,
		public IVideoStreamInfoCollection
	{
	public:
		DecoderPipe(AVStreamInfoCollection stream);
		~DecoderPipe();

		/// <summary>
		///		释放后 read_and_send_frame 函数内如果正在进行循环向消费者送入帧，
		///		则会在当前循环完成后不再执行下一轮循环。
		/// </summary>
		void Dispose() override;

	private:
		AVStreamInfoCollection _stream_infos;
		std::atomic_bool _disposed = false;
		shared_ptr<AVCodecContextWrapper> _decoder;
		AVFrameWrapper _decoder_out_frame;

		void read_and_send_frame();

	public:
		/// <summary>
		///		送入包。会解码，然后将帧送给消费者。
		///		如果没有任何消费者，本函数会直接返回，不会真正去解码，这样可以节省性能。
		///		如果送入的包的流索引和初始化本对象时的流索引不匹配，会直接返回，不会执行解码。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(AVPacketWrapper *packet) override;

		/// <summary>
		///		获取本对象内部的 AVCodecContextWrapper
		/// </summary>
		/// <returns></returns>
		shared_ptr<AVCodecContextWrapper> DecoderContext();

		/// <summary>
		///		冲洗解码器，但是不冲洗消费者。
		/// </summary>
		void FlushDecoderButNotFlushConsumers();

		#pragma region 通过 IAudioStreamInfoCollection 继承
	public:
		AVRational time_base() override;
		void set_time_base(AVRational value) override;
		AVSampleFormat sample_format() override;
		void set_sample_format(AVSampleFormat value) override;
		int sample_rate() override;
		void set_sample_rate(int value) override;
		AVChannelLayout ch_layout() override;
		void set_ch_layout(AVChannelLayout value) override;
		#pragma endregion

		#pragma region 通过 IVideoStreamInfoCollection 继承
	public:
		int width() override;
		void set_width(int value) override;
		int height() override;
		void set_height(int value) override;
		AVPixelFormat pixel_format() override;
		void set_pixel_format(AVPixelFormat value) override;
		AVRational frame_rate() override;
		void set_frame_rate(AVRational value) override;
		#pragma endregion
	};
}