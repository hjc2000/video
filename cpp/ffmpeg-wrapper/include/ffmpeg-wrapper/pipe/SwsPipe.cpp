#include "ffmpeg-wrapper/pipe/SwsPipe.h"

void video::SwsPipe::SendFrame(AVFrameWrapper *frame)
{
	if (!frame)
	{
		// 冲洗模式
		_sws_context->SendFrame(nullptr);
		read_and_send_frame_to_consumer();
		return;
	}

	// 非冲洗模式
	if (*frame != _in_video_frame_infos)
	{
		_in_video_frame_infos = *frame;
		change_sws();
	}

	_sws_context->SendFrame(frame);
	read_and_send_frame_to_consumer();
}
