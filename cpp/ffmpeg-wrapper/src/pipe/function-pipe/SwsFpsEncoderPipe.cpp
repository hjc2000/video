#include"SwsFpsEncoderPipe.h"

using namespace video;

void video::SwsFpsEncoderPipe::SendFrame(AVFrameWrapper *frame)
{
	_sws_pipe->SendFrame(frame);
}
