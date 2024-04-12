#pragma once
#include<atomic>
#include<ffmpeg-wrapper/AVToString.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/PipeFrameSource.h>
#include<ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<jccpp/IDisposable.h>
#include<jccpp/TaskCompletionSignal.h>
#include<jccpp/container/List.h>
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
		AVRational TimeBase() const override;
		void SetTimeBase(AVRational value) override;
		AVSampleFormat sample_format() const override;
		void set_sample_format(AVSampleFormat value) override;
		int SampleRate() const override;
		void SetSampleRate(int value) override;
		AVChannelLayout ChannelLayout() override;
		void SetChannelLayout(AVChannelLayout value) override;
		#pragma endregion

		#pragma region 通过 IVideoStreamInfoCollection 继承
	public:
		int width() override;
		void set_width(int value) override;
		int height() override;
		void set_height(int value) override;
		AVPixelFormat pixel_format() override;
		void set_pixel_format(AVPixelFormat value) override;
		AVRational FrameRate() const override;
		void SetFrameRate(AVRational value) override;
		#pragma endregion
	};
}