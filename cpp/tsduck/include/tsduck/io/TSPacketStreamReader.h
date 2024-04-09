#pragma once
#include<tsduck/interface/ITSPacketSource.h>
#include<jccpp/stream/Stream.h>
#include<tsTSPacketStream.h>

namespace video
{
	/// <summary>
	///		用于从流中读取 ts 包。
	/// </summary>
	class TSPacketStreamReader :public ITSPacketSource
	{
	public:
		/// <summary>
		///		传入一个流。读取包时将会在流中寻找同步字节，解析出 ts 包。能够自适应各种类型的 ts 包。
		/// </summary>
		/// <param name="input_stream"></param>
		TSPacketStreamReader(shared_ptr<Stream> input_stream);

	private:
		shared_ptr<Stream> _input_stream;
		class ReadStreamInterface;
		shared_ptr<ReadStreamInterface> _read_stream_interface;
		shared_ptr<ts::TSPacketStream> _ts_packet_stream;

	public:
		ReadPacketResult ReadPacket(ts::TSPacket &packet) override;
		using ITSPacketSource::PumpTo;
	};
}
