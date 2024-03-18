#pragma once
#include<ITSPacketConsumer.h>
#include<tsTSPacket.h>

namespace video
{
	class ITSPacketSource
	{
	public:
		enum class ReadPacketResult
		{
			Success,

			/// <summary>
			///		没有更多包可以读了。
			/// </summary>
			NoMorePacket
		};

	public:
		virtual ~ITSPacketSource() {}

	public:
		virtual ITSPacketSource::ReadPacketResult ReadPacket(ts::TSPacket &packet) = 0;

		virtual ITSPacketSource::ReadPacketResult PumpTo(shared_ptr<ITsPacketConsumer> consumer)
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
	};
}