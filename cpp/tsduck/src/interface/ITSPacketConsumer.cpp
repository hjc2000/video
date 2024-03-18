#include<ITSPacketConsumer.h>

using namespace video;

void ITsPacketConsumer::SendPacket(std::vector<ts::TSPacket> packets)
{
	for (auto &packet : packets)
	{
		SendPacket(&packet);
	}
}
