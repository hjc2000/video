#include"PidChanger.h"

using namespace video;

void PidChanger::HandlePatVersionChange(ts::PAT &pat)
{
	// 更改 PMT PID。
	for (auto &pmt : pat.pmts)
	{
		// 看映射表里面有没有该 PMT
		auto it = _pid_map.find(pmt.second);
		if (it != _pid_map.end())
		{
			// 更改 PAT 中此 PMT 的 PID。
			pmt.second = it->second;
		}
	}

	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pat));
}

void PidChanger::HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid)
{
	ChangeStreamPidAndPcrPid(pmt);

	// 更改 PMT PID。
	uint16_t out_pmt_pid = source_pid;
	auto it = _pid_map.find(source_pid);
	if (it != _pid_map.end())
	{
		out_pmt_pid = it->second;
	}

	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pmt, out_pmt_pid));
}

void PidChanger::ChangeStreamPidAndPcrPid(ts::PMT &pmt)
{
	// 更改 PCR PID。
	auto it = _pid_map.find(pmt.pcr_pid);
	if (it != _pid_map.end())
	{
		// 如果映射表中找得到 PCR PID，将 PCR PID 改掉。
		pmt.pcr_pid = it->second;
	}

	// 更改各个流的 PID。
	ts::PMT backup_pmt{ pmt };
	for (auto &map_pair : _pid_map)
	{
		auto it = pmt.streams.find(map_pair.first);
		if (it != pmt.streams.end())
		{
			// 如果该 PMT 里有流的 PID 在映射表里找得到，删除该流。
			pmt.streams.erase(it);
		}
	}

	for (auto &map_pair : _pid_map)
	{
		auto it = backup_pmt.streams.find(map_pair.first);
		if (it != backup_pmt.streams.end())
		{
			/* 如果先前备份的 PMT 中有流的 PID 在映射表中找得到，
			将这个流用映射后的 PID 放入原来的 PMT 中。*/
			pmt.streams[map_pair.second] = it->second;
		}
	}
}

void PidChanger::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (_streams_pid_set[packet->getPID()])
	{
		uint16_t src_pid = packet->getPID();
		auto it = _pid_map.find(src_pid);
		if (it != _pid_map.end())
		{
			packet->setPID(it->second);
		}

		SendPacketToEachConsumer(packet);

		// 因为传进来的是指针，刚才把 PID 改了，现在要恢复。
		packet->setPID(src_pid);
	}
	else if (packet->getPID() == 0x11)
	{
		SendPacketToEachConsumer(packet);
	}
}

void PidChanger::SetPidMap(std::map<uint16_t, uint16_t> const &pid_map)
{
	auto it = pid_map.find(0);
	if (it != pid_map.end())
	{
		throw jc::Exception("不允许更改 PAT 的 PID。");
	}

	it = pid_map.find(0x11);
	if (it != pid_map.end())
	{
		throw jc::Exception("不允许更改 SDT 的 PID。");
	}

	_pid_map = pid_map;
}
