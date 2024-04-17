#pragma once
#include<ffmpeg-wrapper/info-collection/IAudioStreamInfoCollection.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/output-format/OutputFormat.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>

namespace video
{
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

		virtual shared_ptr<IFrameConsumer> CreateEncoderPipe(
			std::string codec_name,
			IVideoStreamInfoCollection &in_stream_infos,
			shared_ptr<OutputFormat> output_format,
			int64_t out_bit_rate_in_bps = -1
		);
	};
}