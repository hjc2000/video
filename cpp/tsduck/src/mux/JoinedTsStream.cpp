#include"JoinedTsStream.h"

using namespace video;
using namespace std;

ITSPacketSource::ReadPacketResult video::JoinedTsStream::ReadPacket(ts::TSPacket &packet)
{
	/*
	* 现在这里只是简单地从 _current_ts_packet_source 读出包。实际上读出包后需要进行表格版本号校正。
	* 每次从队列中取出新的 ITSPacketSource 赋值给 _current_ts_packet_source，都需要更新表格版本号。
	*/
	throw jc::NotImplementedException();

	while (1)
	{
		// 如果 _current_ts_packet_source 为空，在这里尝试获取一个新的 ITSPacketSource
		if (_current_ts_packet_source == nullptr)
		{
			try
			{
				if (_ts_packet_source_queue.Count() == 0 && _on_ts_packet_source_list_exhausted != nullptr)
				{
					_on_ts_packet_source_list_exhausted();
				}

				_current_ts_packet_source = _ts_packet_source_queue.Dequeue();
			}
			catch (...)
			{
				cout << CODE_POS_STR << "退队失败，将结束包流" << endl;
			}
		}

		// 尝试获取后仍然为空，结束包流
		if (_current_ts_packet_source == nullptr)
		{
			return ITSPacketSource::ReadPacketResult::NoMorePacket;
		}

		// 到这里说明 _current_ts_packet_source 不为空
		ITSPacketSource::ReadPacketResult read_packet_result = _current_ts_packet_source->ReadPacket(packet);
		if (read_packet_result == ITSPacketSource::ReadPacketResult::Success)
		{
			return ITSPacketSource::ReadPacketResult::Success;
		}

		/* 到这里说明读取失败，将 _current_ts_packet_source 置为空指针，
		下轮循环将尝试取出一个新的 ITSPacketSource*/
		_current_ts_packet_source = nullptr;
	}
}

void video::JoinedTsStream::AddSource(shared_ptr<ITSPacketSource> source)
{
	if (source == nullptr)
	{
		throw jc::ArgumentNullException();
	}

	_ts_packet_source_queue.Enqueue(source);
}
