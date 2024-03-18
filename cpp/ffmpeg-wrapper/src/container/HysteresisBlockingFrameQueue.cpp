#include "HysteresisBlockingFrameQueue.h"

int video::HysteresisBlockingFrameQueue::ReadFrame(AVFrameWrapper &frame)
{
	try
	{
		frame = _frame_queue.Dequeue();
		return 0;
	}
	catch (jc::InvalidOperationException &)
	{
		return (int)ErrorCode::eof;
	}
}

void video::HysteresisBlockingFrameQueue::SendFrame(AVFrameWrapper *frame)
{
	if (!frame)
	{
		_frame_queue.Flush();
		return;
	}

	_frame_queue.Enqueue(*frame);
}
