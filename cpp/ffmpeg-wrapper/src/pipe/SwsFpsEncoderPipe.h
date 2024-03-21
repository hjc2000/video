#pragma once
#include<EncoderPipe.h>
#include<FpsAdjustPipe.h>
#include<OutputFormatBase.h>
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
			shared_ptr<OutputFormatBase> out_fmt_ctx,
			int64_t video_bitrate
		)
		{
			_video_encoder_pipe = shared_ptr<EncoderPipe>{
				new EncoderPipe{
					codec_name,
					desire_out_video_stream_infos,
					out_fmt_ctx,
					video_bitrate
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