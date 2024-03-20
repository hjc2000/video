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

		/// <summary>
		///		将本对象的包泵送给 consumer
		/// </summary>
		/// <param name="consumer"></param>
		/// <returns></returns>
		virtual ITSPacketSource::ReadPacketResult PumpTo(shared_ptr<ITSPacketConsumer> consumer);
	};
}