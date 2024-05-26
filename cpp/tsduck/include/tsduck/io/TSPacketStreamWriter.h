#pragma once
#include<base/stream/Stream.h>
#include<tsduck/interface/ITSPacketConsumer.h>

namespace video
{
	/// <summary>
	///		送入 ts 包，会输出到文件。
	/// </summary>
	class TSPacketStreamWriter :public ITSPacketConsumer
	{
	public:
		TSPacketStreamWriter(shared_ptr<base::Stream> out_stream);

	private:
		shared_ptr<base::Stream> _out_stream;

	public:
		using ITSPacketConsumer::SendPacket;

		/// <summary>
		///		送入包，会被写入文件。
		/// </summary>
		void SendPacket(ts::TSPacket *packet) override;
	};
}
