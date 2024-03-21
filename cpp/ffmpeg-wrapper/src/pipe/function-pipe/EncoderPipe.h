#pragma once
#include<AVCodecContextWrapper.h>
#include<AVStreamWrapper.h>
#include<AudioStreamInfoCollection.h>
#include<IDisposable.h>
#include<IFrameConsumer.h>
#include<IPacketSource.h>
#include<IVideoStreamInfoCollection.h>
#include<OutputFormatBase.h>
#include<VideoStreamInfoCollection.h>
#include<atomic>

namespace video
{
	/// <summary>
	///		* 连接向 OutputFormatBase 的编码管道。构造本类对象，在构造函数中传入 OutputFormatBase 对象，
	///		  会在那个 OutputFormatBase 中创建新的流并与本 EncoderPipe 对象绑定。
	/// 
	///		* 向本对象送入帧后会编码，然后写入 OutputFormat。
	/// </summary>
	class EncoderPipe :public IFrameConsumer
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="codec_name">编码器名称</param>
		/// <param name="in_stream_infos">编码器输入流的信息。</param>
		/// <param name="output_format">编码后的包要写入的格式。</param>
		/// <param name="out_bit_rate_in_bps">想要输出的比特率。设置越大质量越高。设置为 <= 0 表示使用默认值。</param>
		EncoderPipe(
			char const *codec_name,
			IVideoStreamInfoCollection &in_stream_infos,
			shared_ptr<OutputFormatBase> output_format,
			int64_t out_bit_rate_in_bps = -1
		);

		/// <summary>
		///		
		/// </summary>
		/// <param name="codec_name">编码器名称。</param>
		/// <param name="in_stream_infos">编码器输入流信息。</param>
		/// <param name="output_format">编码后的包要写入的封装。</param>
		EncoderPipe(
			char const *codec_name,
			IAudioStreamInfoCollection &in_stream_infos,
			shared_ptr<OutputFormatBase> output_format
		);

	private:
		shared_ptr<AVCodecContextWrapper> _encoder_ctx;
		AVStreamWrapper _new_stream;
		shared_ptr<OutputFormatBase> _output_format;

		/// <summary>
		///		读取包并写入封装
		/// </summary>
		void ReadAndSendPacketToOutputFormat();

	public:
		/**
		 * @brief 送入帧进行编码，然后将包送给消费者。
		 *
		 * @param frame
		 *
		 * @exception SendPacketException 编码后，向封装写入包的过程中有可能发生异常，这取决于消费者。
		*/
		void SendFrame(AVFrameWrapper *frame) override
		{
			// 防止编码器中有数据残留
			ReadAndSendPacketToOutputFormat();
			_encoder_ctx->SendFrame(frame);
			ReadAndSendPacketToOutputFormat();
		}

		/// <summary>
		///		被本编码管道创建并绑定的新流。
		/// </summary>
		/// <returns></returns>
		AVStreamWrapper Stream()
		{
			return _new_stream;
		}
	};
}
