#include"ITSPacketSource.h"

using namespace video;

ITSPacketSource::ReadPacketResult ITSPacketSource::PumpTo(
	shared_ptr<ITSPacketConsumer> consumer,
	shared_ptr<CancellationToken> cancel_pump
)
{
	std::vector<shared_ptr<ITSPacketConsumer>> consumers;
	consumers.push_back(consumer);
	return PumpTo(consumers, cancel_pump);
}

ITSPacketSource::ReadPacketResult ITSPacketSource::PumpTo(
	std::vector<shared_ptr<ITSPacketConsumer>> const consumers,
	shared_ptr<CancellationToken> cancel_pump
)
{
	ts::TSPacket packet;
	while (!cancel_pump->IsCancellationRequested())
	{
		ITSPacketSource::ReadPacketResult read_packet_result = ReadPacket(packet);
		switch (read_packet_result)
		{
		case ITSPacketSource::ReadPacketResult::Success:
			{
				for (auto consumer : consumers)
				{
					if (cancel_pump->IsCancellationRequested())
					{
						return ITSPacketSource::ReadPacketResult::Success;
					}

					consumer->SendPacket(&packet);
				}

				break;
			}
		default:
			{
				return read_packet_result;
			}
		}
	}

	return ITSPacketSource::ReadPacketResult::Success;
}
