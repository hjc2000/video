#include"IPipePacketSource.h"

using namespace video;

void IPipePacketSource::SendPacketToEachConsumer(AVPacketWrapper *packet)
{
	for (shared_ptr<IPacketConsumer> &consumer : PacketConsumerList())
	{
		consumer->SendPacket(packet);
	}
}
