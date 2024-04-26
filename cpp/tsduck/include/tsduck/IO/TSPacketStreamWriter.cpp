#include"tsduck/io/TSPacketStreamWriter.h"

using namespace video;
using namespace std;
using namespace ts;

TSPacketStreamWriter::TSPacketStreamWriter(shared_ptr<Stream> out_stream)
{
	if (out_stream == nullptr)
	{
		throw std::invalid_argument{ "out_stream 不能是空指针" };
	}

	_out_stream = out_stream;
}

void TSPacketStreamWriter::SendPacket(ts::TSPacket *packet)
{
	_out_stream->Write(packet->b, 0, 188);
}
