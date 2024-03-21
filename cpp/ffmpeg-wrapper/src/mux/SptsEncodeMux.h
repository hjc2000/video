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
		)
		{
			_out_format = out_format;
			// 视频参数
			_video_stream_infos = video_stream_infos;
			_video_codec_name = video_codec_name;
			_video_out_bitrate = video_out_bitrate;
			// 音频参数
			_audio_stream_infos = audio_stream_infos;
			_audio_codec_name = audio_codec_name;

			// ts 必须使用 1/90000 时间基
			_video_stream_infos.set_time_base(AVRational(1, 90000));
			_audio_stream_infos.set_time_base(AVRational(1, 90000));

			InitVideoEncodePipe();
			InitAudioEncodePipe();
		}

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

	public:
		shared_ptr<IFrameConsumer> GetVideoEncodePipe();
		shared_ptr<IFrameConsumer> GetAudioEncodePipe();
	};
}
