#include "TSPacketStreamReader.h"

using namespace video;

ITSPacketSource::ReadPacketResult video::TSPacketStreamReader::ReadPacket(ts::TSPacket &packet)
{
	int have_read = _input_stream->ReadExactly(packet.b, 0, 188);
	if (have_read < 188)
	{
		return ITSPacketSource::ReadPacketResult::NoMorePacket;
	}

	return ITSPacketSource::ReadPacketResult::Success;
}