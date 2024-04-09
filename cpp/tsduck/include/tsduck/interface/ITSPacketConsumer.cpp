#include"ITSPacketConsumer.h"

using namespace video;

void ITSPacketConsumer::SendPacket(std::vector<ts::TSPacket> packets)
{
	for (auto &packet : packets)
	{
		SendPacket(&packet);
	}
}
