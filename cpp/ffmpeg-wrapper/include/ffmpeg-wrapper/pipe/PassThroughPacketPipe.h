#pragma once
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/PipePacketSource.h>

namespace video
{
	/**
	 * @brief 透传的包管道。送进来的包不会经过任何加工，直接送给消费者。
	*/
	class PassThroughPacketPipe :public IPacketConsumer, public PipePacketSource
	{
	public:
		void SendPacket(AVPacketWrapper *packet) override
		{
			SendPacketToEachConsumer(packet);
		}
	};
}