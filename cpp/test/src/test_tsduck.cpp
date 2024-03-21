#include "test_tsduck.h"
#include<FileStream.h>
#include<JoinedTsStream.h>
#include<PatParser.h>
#include<TSPacketStreamReader.h>
#include<filesystem>
#include<tsCerrReport.h>

using namespace std;
using namespace video;

void test_tsduck()
{
	// 输入文件
	Queue<string> file_queue;
	file_queue.Enqueue("不老梦.ts");
	file_queue.Enqueue("水龙吟.ts");
	file_queue.Enqueue("idol.ts");

	JoinedTsStream joined_ts_stream;
	joined_ts_stream._on_ts_packet_source_list_exhausted = [&]()
	{
		string file_name;
		if (!file_queue.TryDequeue(file_name))
		{
			return;
		}

		shared_ptr<FileStream> input_file_stream = FileStream::Open(file_name.c_str());
		shared_ptr<TSPacketStreamReader> ts_packet_reader{ new TSPacketStreamReader{input_file_stream} };
		joined_ts_stream.AddSource(ts_packet_reader);
	};

	shared_ptr<PatParser> pat_parset{ new PatParser{} };
	ITSPacketSource::ReadPacketResult pump_result = joined_ts_stream.PumpTo(pat_parset);
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
