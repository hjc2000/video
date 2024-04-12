#include "SwsFpsPipe.h"
#include<FpsAdjustPipe.h>
#include<SwsPipe.h>

using namespace video;

List<shared_ptr<IFrameConsumer>> &SwsFpsPipe::ConsumerList()
{
	return _fps_adjust_pipe->ConsumerList();
}

video::SwsFpsPipe::SwsFpsPipe(IVideoStreamInfoCollection &out_video_stream_infos)
{
	_fps_adjust_pipe = shared_ptr<FpsAdjustPipe>{
		new FpsAdjustPipe {
			out_video_stream_infos,
			out_video_stream_infos.FrameRate()
		}
	};
	_sws_pipe = shared_ptr<SwsPipe>{ new SwsPipe {out_video_stream_infos} };
	_sws_pipe->ConsumerList().Add(_fps_adjust_pipe);
}

void video::SwsFpsPipe::SendFrame(AVFrameWrapper *frame)
{
	_sws_pipe->SendFrame(frame);
}
