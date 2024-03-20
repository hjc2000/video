#include"ITSPacketSource.h"

using namespace video;

ITSPacketSource::ReadPacketResult ITSPacketSource::PumpTo(shared_ptr<ITSPacketConsumer> consumer)
{
	ts::TSPacket packet;
	while (true)
	{
		ITSPacketSource::ReadPacketResult read_packet_result = ReadPacket(packet);
		switch (read_packet_result)
		{
		case ITSPacketSource::ReadPacketResult::Success:
			{
				consumer->SendPacket(&packet);
				break;
			}
		default:
			{
				return read_packet_result;
			}
		}
	}
}