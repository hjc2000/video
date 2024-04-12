#include"ffmpeg-wrapper/pipe/interface/PipePacketSource.h"

using namespace video;

void PipePacketSource::SendPacketToEachConsumer(AVPacketWrapper *packet)
{
	for (shared_ptr<IPacketConsumer> &consumer : _consumer_list)
	{
		consumer->SendPacket(packet);
	}
}

void PipePacketSource::AddPacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer)
{
	if (!packet_comsumer)
	{
		cout << CODE_POS_STR << "添加包消费者时传入了空指针" << endl;
		return;
	}

	_consumer_list.Add(packet_comsumer);
}

bool PipePacketSource::RemovePacketConsumer(shared_ptr<IPacketConsumer> packet_comsumer)
{
	if (!packet_comsumer)
	{
		return false;
	}

	return _consumer_list.Remove(packet_comsumer);
}

void PipePacketSource::ClearPacketConsumer()
{
	_consumer_list.Clear();
}
