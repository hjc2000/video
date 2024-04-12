#include"ffmpeg-wrapper/pipe/interface/PipeFrameSource.h"

using namespace video;

void IPipeFrameSource::SendFrameToEachConsumer(AVFrameWrapper *frame)
{
	for (shared_ptr<IFrameConsumer> &consumer : ConsumerList())
	{
		consumer->SendFrame(frame);
	}
}
