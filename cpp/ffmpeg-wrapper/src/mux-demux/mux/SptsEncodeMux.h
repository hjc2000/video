#pragma once
#include<AudioStreamInfoCollection.h>
#include<OutputFormat.h>
#include<SwrEncoderPipe.h>
#include<SwsFpsEncoderPipe.h>
#include<VideoStreamInfoCollection.h>

namespace video
{
	/// <summary>
	///		输入视频帧和音频帧，将会编码，然后封装成 ts。
	/// 
	///		本类能够自适应输入的视频帧、音频帧的参数变化。例如：音频帧的采样格式变了之后能够重新构造
	///		重采样器，从而保持输出端的音频帧格式始终不变。
	/// </summary>
	class SptsEncodeMux
	{
	public:
		SptsEncodeMux(
			shared_ptr<OutputFormat> out_format,
			// 视频相关参数
			IVideoStreamInfoCollection &video_stream_infos,
			std::string video_codec_name,
			int64_t video_out_bitrate,
			// 音频相关参数
			IAudioStreamInfoCollection &audio_stream_infos,
			std::string audio_codec_name
		);

	private:
		shared_ptr<OutputFormat> _out_format;

		VideoStreamInfoCollection _video_stream_infos;
		shared_ptr<SwsFpsEncoderPipe> _video_encode_pipe;
		std::string _video_codec_name;
		int64_t _video_out_bitrate = -1;

		AudioStreamInfoCollection _audio_stream_infos;
		shared_ptr<SwrEncoderPipe> _audio_encode_pipe;
		std::string _audio_codec_name;

		void InitVideoEncodePipe();
		void InitAudioEncodePipe();
		void WriteHeader();

	public:
		shared_ptr<IFrameConsumer> VideoEncodePipe();
		shared_ptr<IFrameConsumer> AudioEncodePipe();
	};
}

void test_SptsEncodeMux();
