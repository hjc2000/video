#pragma once
#include<memory>
#include<tsTSPacket.h>
#include<vector>

namespace video
{
	using std::shared_ptr;

	class ITsPacketConsumer
	{
	public:
		virtual ~ITsPacketConsumer() {}

	public:
		virtual void SendPacket(ts::TSPacket *packet) = 0;

		void SendPacket(std::vector<ts::TSPacket> packets)
		{
			for (auto &packet : packets)
			{
				SendPacket(&packet);
			}
		}
	};
}