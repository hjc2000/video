#pragma once
#include<ITSPacketConsumer.h>
#include<ITSPacketSource.h>
#include<TableVersionChangeHandler.h>

namespace video
{
	class TableVersionChanger :public ITSPacketConsumer, public ITSPacketSource, public TableVersionChangeHandler
	{
	public:
		// 通过 ITSPacketConsumer 继承
		void SendPacket(ts::TSPacket *packet) override;

		// 通过 ITSPacketSource 继承
		ITSPacketSource::ReadPacketResult ReadPacket(ts::TSPacket &packet) override;
	};
}
