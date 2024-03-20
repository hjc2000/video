#include"PipeTsPacketSource.h"

using namespace video;
using namespace std;

void video::PipeTsPacketSource::SendPacketToEachConsumer(ts::TSPacket *packet)
{
	for (shared_ptr<ITSPacketConsumer> &consumer : _consumer_list)
	{
		consumer->SendPacket(packet);
	}
}

void video::PipeTsPacketSource::SendPacketToEachConsumer(std::vector<ts::TSPacket> packets)
{
	for (auto &packet : packets)
	{
		SendPacketToEachConsumer(&packet);
	}
}

void video::PipeTsPacketSource::SendPacketToEachConsumer(std::vector<std::vector<ts::TSPacket>> packet_vectors)
{
	for (auto &it : packet_vectors)
	{
		SendPacketToEachConsumer(it);
	}
}

void video::PipeTsPacketSource::AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	if (!packet_comsumer)
	{
		cout << CODE_POS_STR << "添加包消费者时传入了空指针" << endl;
		return;
	}

	_consumer_list.Add(packet_comsumer);
}

bool video::PipeTsPacketSource::RemovePacketComsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	if (!packet_comsumer)
	{
		return false;
	}

	return _consumer_list.Remove(packet_comsumer);
}

void video::PipeTsPacketSource::AddTsPacketConsumerFromAnother(PipeTsPacketSource &another)
{
	for (shared_ptr<ITSPacketConsumer> &consumer : another._consumer_list)
	{
		AddTsPacketConsumer(consumer);
	}
}

void video::PipeTsPacketSource::ClearConsumers()
{
	_consumer_list.Clear();
}
