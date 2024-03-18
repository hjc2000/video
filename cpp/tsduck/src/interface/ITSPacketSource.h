#pragma once
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
	};
}