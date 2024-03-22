#pragma once
#include<AudioStreamInfoCollection.h>
#include<OutputFormat.h>
#include<SwrEncoderPipe.h>
#include<SwsFpsEncoderPipe.h>
#include<VideoStreamInfoCollection.h>

namespace video
{
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
		shared_ptr<IFrameConsumer> GetVideoEncodePipe();
		shared_ptr<IFrameConsumer> GetAudioEncodePipe();
	};
}

void test_SptsEncodeMux();
