#include "test_tsduck.h"
#include<FileStream.h>
#include<PatParser.h>
#include<TSPacketStreamReader.h>
#include<filesystem>
#include<tsCerrReport.h>

using namespace std;
using namespace video;

void test_tsduck()
{
	// 输入文件
	shared_ptr<FileStream> input_file_stream = FileStream::Open("fallen-down.ts");
	TSPacketStreamReader ts_packet_reader{ input_file_stream };
	shared_ptr<PatParser> pat_handler{ new PatParser{} };

	ITSPacketSource::ReadPacketResult pump_result = ts_packet_reader.PumpTo(pat_handler);
	switch (pump_result)
	{
	case ITSPacketSource::ReadPacketResult::NoMorePacket:
		{
			cout << "读取完成，没有更多包了" << endl;
			break;
		}
	default:
		{
			cout << "其他错误" << endl;
			break;
		}
	}
}
