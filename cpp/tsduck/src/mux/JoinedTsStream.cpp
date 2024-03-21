#include"JoinedTsStream.h"
#include<TableOperator.h>

using namespace video;
using namespace std;

#pragma region 内部类
class JoinedTsStream::TableVersionChanger :
	public ITSPacketConsumer,
	public ITSPacketSource,
	public TableHandler
{
private:
	TSPacketQueue _ts_packet_queue;
	uint8_t _table_version_offset = 0;
	ts::PIDSet _streams_pid_set;

public:
	void HandlePAT(ts::BinaryTable const &table) override
	{
		ts::PAT pat;
		pat.deserialize(*_duck, table);
		_streams_pid_set.reset();
		ResetListenedPids();
		ListenOnPmtPids(pat);
		pat.version += _table_version_offset;
		_ts_packet_queue.SendPacket(TableOperator::ToTsPacket(*_duck, pat));
		_demux->reset();
	}

	void HandlePMT(ts::BinaryTable const &table) override
	{
		ts::PMT pmt;
		pmt.deserialize(*_duck, table);
		_streams_pid_set << pmt;
		pmt.version += _table_version_offset;
		_ts_packet_queue.SendPacket(TableOperator::ToTsPacket(*_duck, pmt, table.sourcePID()));
		_demux->reset();
	}

	void HandleSDT(ts::BinaryTable const &table) override
	{
		ts::SDT sdt;
		sdt.deserialize(*_duck, table);
		sdt.version += _table_version_offset;
		_ts_packet_queue.SendPacket(TableOperator::ToTsPacket(*_duck, sdt));
		_demux->reset();
	}

	void SendPacket(ts::TSPacket *packet) override
	{
		if (packet == nullptr)
		{
			// 冲洗内部队列
			_ts_packet_queue.SendPacket(packet);
			return;
		}

		_demux->feedPacket(*packet);
		switch (packet->getPID())
		{
		case 0:
			{
				break;
			}
		case 0x11:
			{
				break;
			}
		default:
			{
				if (_streams_pid_set[packet->getPID()])
				{
					_ts_packet_queue.SendPacket(packet);
				}

				break;
			}
		}
	}

	ITSPacketSource::ReadPacketResult ReadPacket(ts::TSPacket &packet) override
	{
		return _ts_packet_queue.ReadPacket(packet);
	}

	void IncreaseVersion()
	{
		cout << "递增版本号" << endl;
		_table_version_offset++;
		_streams_pid_set.reset();
		ResetListenedPids();
		_demux->reset();
	}
};
#pragma endregion


JoinedTsStream::JoinedTsStream()
{
	_table_version_changer = shared_ptr<TableVersionChanger>{ new TableVersionChanger{} };
}

ITSPacketSource::ReadPacketResult video::JoinedTsStream::ReadPacket(ts::TSPacket &packet)
{
	while (1)
	{
		TryGetNextSourceIfNullAndIncreaseVersion();
		if (_current_ts_packet_source == nullptr)
		{
			// 尝试获取后仍然为空，结束包流
			return ITSPacketSource::ReadPacketResult::NoMorePacket;
		}

		// 到这里说明 _current_ts_packet_source 不为空
		ITSPacketSource::ReadPacketResult read_packet_result = _current_ts_packet_source->ReadPacket(packet);
		if (read_packet_result != ITSPacketSource::ReadPacketResult::Success)
		{
			// 读取失败，进入下一轮循环
			_current_ts_packet_source = nullptr;
			continue;
		}

		_table_version_changer->SendPacket(&packet);
		read_packet_result = _table_version_changer->ReadPacket(packet);
		if (read_packet_result == ITSPacketSource::ReadPacketResult::Success)
		{
			return ITSPacketSource::ReadPacketResult::Success;
		}
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

/// <summary>
///		尝试获取下一个 ITSPacketSource。
///		需要先将 _current_ts_packet_source 赋值为空指针，否则检测到 _current_ts_packet_source
///		不为空指针，会直接返回，不进行任何操作。
/// </summary>
void video::JoinedTsStream::TryGetNextSourceIfNullAndIncreaseVersion()
{
	if (_current_ts_packet_source != nullptr)
	{
		return;
	}

	try
	{
		if (_ts_packet_source_queue.Count() == 0 && _on_ts_packet_source_list_exhausted != nullptr)
		{
			_on_ts_packet_source_list_exhausted();
		}

		_current_ts_packet_source = _ts_packet_source_queue.Dequeue();
		_table_version_changer->IncreaseVersion();
	}
	catch (...)
	{
		cout << CODE_POS_STR << "退队失败，将结束包流" << endl;
	}
}
