#include"JoinedTsStream.h"

using namespace video;
using namespace std;

ITSPacketSource::ReadPacketResult video::JoinedTsStream::ReadPacket(ts::TSPacket &packet)
{
	if (_current_ts_packet_source == nullptr)
	{

	}

	return ITSPacketSource::ReadPacketResult();
}

void video::JoinedTsStream::AddSource(shared_ptr<ITSPacketSource> source)
{
	if (source == nullptr)
	{
		throw jc::ArgumentNullException();
	}

	_ts_packet_source_queue.Enqueue(source);
}
