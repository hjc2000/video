#pragma once
#include<ffmpeg-wrapper/factory/EncoderPipeFactory.h>
#include<ffmpeg-wrapper/pipe/SwsFpsPipe.h>

namespace video
{
	class SwsFpsEncoderPipe :public IFrameConsumer
	{
		shared_ptr<SwsFpsPipe> _sws_fps_pipe;
		shared_ptr<IFrameConsumer> _video_encode_pipe;

	public:
		SwsFpsEncoderPipe(
			shared_ptr<OutputFormat> out_format,
			IVideoStreamInfoCollection const &video_stream_infos,
			std::string video_codec_name,
			int64_t video_out_bitrate_in_bps
		);

		void SendFrame(AVFrameWrapper *frame) override;
	};
}