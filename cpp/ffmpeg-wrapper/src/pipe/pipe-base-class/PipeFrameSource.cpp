#include"PipeFrameSource.h"

using namespace video;

void PipeFrameSource::SendFrameToEachConsumer(AVFrameWrapper *frame)
{
	for (shared_ptr<IFrameConsumer> &consumer : _consumer_list)
	{
		consumer->SendFrame(frame);
	}
}

void PipeFrameSource::AddFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer)
{
	if (!frame_consumer)
	{
		cout << CODE_POS_STR << "添加帧消费者时传入了空指针" << endl;
		return;
	}

	_consumer_list.Add(frame_consumer);
}

bool PipeFrameSource::RemoveFrameConsumer(shared_ptr<IFrameConsumer> frame_consumer)
{
	if (!frame_consumer)
	{
		return false;
	}

	return _consumer_list.Remove(frame_consumer);
}

void PipeFrameSource::AddConsumerFromAnotherPipe(PipeFrameSource &another)
{
	if (this == &another)
	{
		cout << CODE_POS_STR << "禁止复制自己的消费者列表" << endl;
		throw jc::InvalidOperationException();
	}

	for (shared_ptr<IFrameConsumer> &consumer : another._consumer_list)
	{
		_consumer_list.Add(consumer);
	}
}
