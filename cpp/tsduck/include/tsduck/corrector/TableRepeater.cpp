#include"tsduck/corrector/TableRepeater.h"

using namespace std;

void video::TableRepeater::HandlePatVersionChange(ts::PAT &pat)
{
	_pat_packets = TableOperator::ToTsPacket(*_duck, pat);
	SendPacketToEachConsumer(_pat_packets);

	// PMT 要重新解析
	_pmt_packet_vectors.clear();
	_sdt_packets.clear();
}

void video::TableRepeater::HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid)
{
	std::vector<ts::TSPacket> pmt_pckets = TableOperator::ToTsPacket(*_duck, pmt, source_pid);
	_pmt_packet_vectors.push_back(pmt_pckets);
	SendPacketToEachConsumer(pmt_pckets);
}

void video::TableRepeater::HandleSdtVersionChange(ts::BinaryTable const &table)
{
	ts::SDT sdt;
	sdt.deserialize(*_duck, table);
	_sdt_packets = TableOperator::ToTsPacket(*_duck, sdt);
	SendPacketToEachConsumer(_sdt_packets);
}

void video::TableRepeater::SendTable()
{
	SendPacketToEachConsumer(_pat_packets);
	SendPacketToEachConsumer(_sdt_packets);
	SendPacketToEachConsumer(_pmt_packet_vectors);
}

void video::TableRepeater::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (packet->hasPCR())
	{
		// PCR 每 100 毫秒发送一次。
		uint64_t _current_pcr = packet->getPCR();

		// diff_pcr_at_send_pat = _current_pcr - _pcr_at_last_send_pat_pmt
		uint64_t diff_pcr_at_send_pat = ts::DiffPCR(_pcr_at_last_send_pat_pmt, _current_pcr);
		int64_t millisecond = ts::PCRToMilliSecond(diff_pcr_at_send_pat);
		if (millisecond >= _repeat_table_interval_in_milliseconds)
		{
			SendTable();
			_pcr_at_last_send_pat_pmt = _current_pcr;
		}
	}

	if (_streams_pid_set[packet->getPID()])
	{
		SendPacketToEachConsumer(packet);
	}
}

int64_t video::TableRepeater::RepeatPatPmtIntervalInMillisecond()
{
	return _repeat_table_interval_in_milliseconds;
}

void video::TableRepeater::SetRepeatPatPmtIntervalInMillisecond(int64_t value)
{
	if (value <= 0)
	{
		std::cout << CODE_POS_STR << "设置的间隔不允许小于等于 0." << std::endl;
		throw jc::ArgumentException("设置的间隔不允许小于等于 0.");
	}

	_repeat_table_interval_in_milliseconds = value;
}
