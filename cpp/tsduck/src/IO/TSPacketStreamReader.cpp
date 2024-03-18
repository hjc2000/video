#include "TSPacketStreamReader.h"

using namespace video;

video::TSPacketStreamReader::TSPacketStreamReader(shared_ptr<Stream> input_stream)
{
	_input_stream = input_stream;
}

ITSPacketSource::ReadPacketResult video::TSPacketStreamReader::ReadPacket(ts::TSPacket &packet)
{
	int64_t have_read = _input_stream->ReadExactly(packet.b, 0, 188);
	if (have_read < 188)
	{
		return ITSPacketSource::ReadPacketResult::NoMorePacket;
	}

	return ITSPacketSource::ReadPacketResult::Success;
}