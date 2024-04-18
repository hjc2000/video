#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>

namespace video
{
	/// <summary>
	///		编码管道工厂。
	///		编码管道应该自己连接着输出格式，在构造函数中就要求传入输出格式，初始化后应该
	///		自动在输出格式中创建新的流，输入帧，编码后自动将包写入输出格式。
	/// </summary>
	class EncoderPipeFactory
	{
	public:
		#pragma region 单例
		static EncoderPipeFactory &Instance();

		/// <summary>
		///		派生此类，将实例赋值给本字段，可以让 Instance 方法返回自定义的工厂。
		/// </summary>
		static shared_ptr<EncoderPipeFactory> _custom_factory;
		#pragma endregion

		/// <summary>
		///		构造视频编码管道
		/// </summary>
		/// <param name="codec_name"></param>
		/// <param name="in_stream_infos"></param>
		/// <param name="output_format"></param>
		/// <param name="out_bit_rate_in_bps"></param>
		/// <returns></returns>
		virtual shared_ptr<IFrameConsumer> CreateEncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection const &in_stream_infos,
			shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1
		);

		/// <summary>
		///		构造音频编码管道
		/// </summary>
		/// <param name="codec_name"></param>
		/// <param name="in_stream_infos"></param>
		/// <param name="output_format"></param>
		/// <returns></returns>
		virtual shared_ptr<IFrameConsumer> CreateEncoderPipe(
			std::string codec_name,
			IAudioStreamInfoCollection const &in_stream_infos,
			shared_ptr<OutputFormat> output_format
		);
	};
}