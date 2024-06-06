#pragma once
#include<base/task/CancellationToken.h>
#include<tsduck/interface/ITSPacketConsumer.h>
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
			NoMorePacket,

			/// <summary>
			///		需要更多的输入数据才能继续读出包。
			/// </summary>
			NeedMoreInput,
		};

	public:
		virtual ~ITSPacketSource() { }

	public:
		virtual ITSPacketSource::ReadPacketResult ReadPacket(ts::TSPacket &packet) = 0;

		/// <summary>
		///		在循环中从本对象的 ReadPacket 方法读出包送给 consumer。
		///		遇到非 ReadPacketResult::Success 的情况会返回该 ReadPacketResult 类型的值。
		/// 
		///		取消后会返回 ITSPacketSource::ReadPacketResult::Success
		/// </summary>
		/// <param name="consumer"></param>
		/// <returns></returns>
		virtual ITSPacketSource::ReadPacketResult PumpTo(
			shared_ptr<ITSPacketConsumer> consumer,
			shared_ptr<base::CancellationToken> cancel_pump
		);

		virtual ITSPacketSource::ReadPacketResult PumpTo(
			std::vector<shared_ptr<ITSPacketConsumer>> const consumers,
			shared_ptr<base::CancellationToken> cancel_pump
		);
	};
}