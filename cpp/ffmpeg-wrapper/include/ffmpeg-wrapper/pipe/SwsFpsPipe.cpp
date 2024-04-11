#include "SwsFpsPipe.h"

using namespace video;

video::SwsFpsPipe::SwsFpsPipe(IVideoStreamInfoCollection &out_video_stream_infos)
{
	_fps_adjust_pipe = shared_ptr<FpsAdjustPipe>{
		new FpsAdjustPipe {
			out_video_stream_infos,
			out_video_stream_infos.frame_rate()
		}
	};
	_sws_pipe = shared_ptr<SwsPipe>{ new SwsPipe {out_video_stream_infos} };
	_sws_pipe->AddFrameConsumer(_fps_adjust_pipe);
}

void video::SwsFpsPipe::AddFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer)
{
	_fps_adjust_pipe->AddFrameConsumer(frame_consumer);
}

bool video::SwsFpsPipe::RemoveFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer)
{
	return _fps_adjust_pipe->RemoveFrameConsumer(frame_consumer);
}

void video::SwsFpsPipe::ClearFrameConsumer()
{
	_fps_adjust_pipe->ClearFrameConsumer();
}
