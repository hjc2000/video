#include"ffmpeg-wrapper/pipe/SwsFpsEncoderPipe.h"

using namespace video;

SwsFpsEncoderPipe::SwsFpsEncoderPipe(
	std::string codec_name,
	IVideoStreamInfoCollection &desire_out_video_stream_infos,
	shared_ptr<OutputFormat> out_fmt_ctx,
	int64_t video_out_bitrate_in_bps
)
{
	_video_encoder_pipe = shared_ptr<EncoderPipe>{
		new EncoderPipe{
			codec_name.c_str(),
			desire_out_video_stream_infos,
			out_fmt_ctx,
			video_out_bitrate_in_bps
		}
	};

	_fps_adjust_pipe = shared_ptr<FpsAdjustPipe>{
		new FpsAdjustPipe {
			desire_out_video_stream_infos,
			desire_out_video_stream_infos.frame_rate()
		}
	};
	_fps_adjust_pipe->AddFrameConsumer(_video_encoder_pipe);

	_sws_pipe = shared_ptr<SwsPipe>{ new SwsPipe {desire_out_video_stream_infos} };
	_sws_pipe->AddFrameConsumer(_fps_adjust_pipe);
}

void video::SwsFpsEncoderPipe::SendFrame(AVFrameWrapper *frame)
{
	_sws_pipe->SendFrame(frame);
}
