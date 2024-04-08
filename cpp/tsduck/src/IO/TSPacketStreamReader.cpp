#include "TSPacketStreamReader.h"
#include<jccpp/define.h>
#include<tsAbstractReadStreamInterface.h>

using namespace video;
using namespace ts;
using namespace std;

#pragma region 内部类型
/// <summary>
///		让 tsduck 读取字节流的接口
/// </summary>
class TSPacketStreamReader::ReadStreamInterface :public ts::AbstractReadStreamInterface
{
public:
	ReadStreamInterface(shared_ptr<Stream> input_stream)
	{
		_input_stream = input_stream;
	}

private:
	shared_ptr<Stream> _input_stream;
	bool _end_of_stream = false;

public:
	bool readStreamPartial(void *addr, size_t max_size, size_t &ret_size, Report &report) override
	{
		try
		{
			int64_t have_read = _input_stream->Read((uint8_t *)addr, 0, max_size);
			ret_size = have_read;
			if (have_read == 0)
			{
				_end_of_stream = true;
			}

			return true;
		}
		catch (std::exception &e)
		{
			cerr << CODE_POS_STR << e.what() << endl;
			return false;
		}
	}

	bool endOfStream() override
	{
		return _end_of_stream;
	}
};
#pragma endregion

video::TSPacketStreamReader::TSPacketStreamReader(shared_ptr<Stream> input_stream)
{
	_input_stream = input_stream;
	_read_stream_interface = shared_ptr<ReadStreamInterface>{ new ReadStreamInterface{_input_stream} };
	_ts_packet_stream = shared_ptr<ts::TSPacketStream>{
		new ts::TSPacketStream{
			ts::TSPacketFormat::AUTODETECT,
			_read_stream_interface.get(),
			nullptr
		}
	};
}

ITSPacketSource::ReadPacketResult video::TSPacketStreamReader::ReadPacket(ts::TSPacket &packet)
{
	int64_t have_read = _ts_packet_stream->readPackets(&packet, nullptr, 1, CerrReport::Instance());
	if (have_read == 0)
	{
		return ITSPacketSource::ReadPacketResult::NoMorePacket;
	}

	return ITSPacketSource::ReadPacketResult::Success;
}
