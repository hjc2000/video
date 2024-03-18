#pragma once
#include<ITSPacketSource.h>
#include<Stream.h>

namespace video
{
	/// <summary>
	///		用于从流中读取 ts 包。
	/// </summary>
	class TSPacketStreamReader :ITSPacketSource
	{
	public:
		TSPacketStreamReader(shared_ptr<Stream> input_stream);

	private:
		shared_ptr<Stream> _input_stream;

	public:
		ReadPacketResult ReadPacket(ts::TSPacket &packet) override;

		using ITSPacketSource::PumpTo;
	};
}