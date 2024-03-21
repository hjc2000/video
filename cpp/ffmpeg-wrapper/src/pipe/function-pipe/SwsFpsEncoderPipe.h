#pragma once
#include<EncoderPipe.h>
#include<FpsAdjustPipe.h>
#include<OutputFormat.h>
#include<SwsPipe.h>
#include<VideoStreamInfoCollection.h>

namespace video
{
	class SwsFpsEncoderPipe :
		public IFrameConsumer
	{
	public:
		SwsFpsEncoderPipe(
			char const *codec_name,
			IVideoStreamInfoCollection &desire_out_video_stream_infos,
			shared_ptr<OutputFormat> out_fmt_ctx,
			int64_t video_bitrate
		);

	private:
		shared_ptr<EncoderPipe> _video_encoder_pipe;
		shared_ptr<SwsPipe> _sws_pipe;
		shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		AVStreamWrapper NewVideoStream()
		{
			return _video_encoder_pipe->Stream();
		}

		void SendFrame(AVFrameWrapper *frame) override;
	};
}