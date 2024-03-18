#pragma once
#include<EncoderPipe.h>
#include<FpsAdjustPipe.h>
#include<OutputFormat.h>
#include<SwsPipe.h>
#include<VideoFrameDisplayer.h>
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
			int64_t video_bitrate,
			bool display_sws_out_frame
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

			if (display_sws_out_frame)
			{
				VideoFrameDisplayer::Options options{ desire_out_video_stream_infos };
				options._x = 0;
				options._y = 70;
				_displayer = shared_ptr<VideoFrameDisplayer>{ new VideoFrameDisplayer{options} };
				_fps_adjust_pipe->AddFrameConsumer(_displayer);
			}

		}

	private:
		shared_ptr<EncoderPipe> _video_encoder_pipe;
		shared_ptr<SwsPipe> _sws_pipe;
		shared_ptr<VideoFrameDisplayer> _displayer;
		shared_ptr<FpsAdjustPipe> _fps_adjust_pipe;

	public:
		AVStreamWrapper NewVideoStream()
		{
			return _video_encoder_pipe->Stream();
		}

		void SendFrame(AVFrameWrapper *frame) override;
	};
}