#pragma once
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<jccpp/container/List.h>

namespace video
{
	class IPipePacketSource
	{
	public:
		virtual ~IPipePacketSource() = default;

		virtual List<shared_ptr<IPacketConsumer>> &PacketConsumerList() = 0;

		/// <summary>
		///		向每个消费者送入包
		/// </summary>
		/// <param name="packet"></param>
		void SendPacketToEachConsumer(AVPacketWrapper *packet);
	};
}