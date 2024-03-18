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
	shared_ptr<FileStream> input_file_stream = FileStream::Open("不老梦.ts");
	TSPacketStreamReader ts_packet_reader{ input_file_stream };
	PatParser pat_handler;
	ts::TSPacket packet;

	auto loop_read_packet = [&]()
	{
		while (true)
		{
			ITSPacketSource::ReadPacketResult read_packet_result = ts_packet_reader.ReadPacket(packet);
			switch (read_packet_result)
			{
			case ITSPacketSource::ReadPacketResult::Success:
				{
					pat_handler.SendPacket(&packet);
					break;
				}
			default:
				{
					cout << "ReadPacket 返回错误代码，结束读取" << endl;
					return;
				}
			}
		}
	};

	loop_read_packet();
}