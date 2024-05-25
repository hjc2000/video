#include "test_tsduck.h"
#include<filesystem>
#include<jccpp/stream/FileStream.h>
#include<tsCerrReport.h>
#include<tsduck/io/TSPacketStreamReader.h>
#include<tsduck/mux/JoinedTsStream.h>
#include<tsduck/TestProgramMux.h>

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

		shared_ptr<jccpp::FileStream> input_file_stream = jccpp::FileStream::Open(file_name.c_str());
		shared_ptr<TSPacketStreamReader> ts_packet_reader { new TSPacketStreamReader { input_file_stream } };
		joined_ts_stream.AddSource(ts_packet_reader);
	};

	shared_ptr<TestProgramMux> test_program_mux { new TestProgramMux { } };
	CancellationTokenSource cancel_pump_source;
	ITSPacketSource::ReadPacketResult pump_result = joined_ts_stream.PumpTo(test_program_mux, cancel_pump_source.Token());
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
