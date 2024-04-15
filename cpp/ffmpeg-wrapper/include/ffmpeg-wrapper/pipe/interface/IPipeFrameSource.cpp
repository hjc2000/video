#include"ffmpeg-wrapper/pipe/interface/IPipeFrameSource.h"

using namespace video;

void IPipeFrameSource::SendFrameToEachConsumer(AVFrameWrapper *frame)
{
	for (shared_ptr<IFrameConsumer> &consumer : FrameConsumerList())
	{
		consumer->SendFrame(frame);
	}
}