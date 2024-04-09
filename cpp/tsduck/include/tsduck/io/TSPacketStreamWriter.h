#pragma once
#include<tsduck/interface/ITSPacketConsumer.h>
#include<jccpp/stream/Stream.h>

namespace video
{
	/// <summary>
	///		送入 ts 包，会输出到文件。
	/// </summary>
	class TSPacketStreamWriter :public ITSPacketConsumer
	{
	public:
		TSPacketStreamWriter(shared_ptr<Stream> out_stream);

	private:
		shared_ptr<Stream> _out_stream;

	public:
		using ITSPacketConsumer::SendPacket;

		/// <summary>
		///		送入包，会被写入文件。
		/// </summary>
		void SendPacket(ts::TSPacket *packet) override;
	};
}
