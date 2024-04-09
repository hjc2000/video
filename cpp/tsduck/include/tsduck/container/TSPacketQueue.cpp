#include"tsduck/container/TSPacketQueue.h"

using namespace video;
using namespace std;

void video::TSPacketQueue::SendPacket(ts::TSPacket *packet)
{
	if (_flushed)
	{
		throw jc::InvalidOperationException("已经冲洗了，禁止再送入包");
	}

	if (packet == nullptr)
	{
		_flushed = true;
		return;
	}

	_packet_queue.Enqueue(*packet);
}

ITSPacketSource::ReadPacketResult video::TSPacketQueue::ReadPacket(ts::TSPacket &packet)
{
	bool dequeue_result = _packet_queue.TryDequeue(packet);
	if (dequeue_result)
	{
		return ITSPacketSource::ReadPacketResult::Success;
	}

	// 退队失败
	if (_flushed)
	{
		return ITSPacketSource::ReadPacketResult::NoMorePacket;
	}

	return ITSPacketSource::ReadPacketResult::NeedMoreInput;
}
