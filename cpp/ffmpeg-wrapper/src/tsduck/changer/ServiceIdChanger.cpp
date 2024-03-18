#include"ServiceIdChanger.h"

using namespace video;
using namespace std;

video::ServiceIdChanger::ServiceIdChanger(std::map<uint16_t, uint16_t> service_id_map)
{
	_service_id_map = service_id_map;
}

void video::ServiceIdChanger::HandlePatVersionChange(ts::PAT &pat)
{
	ChangeServiceIdInPat(pat);
	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pat));
}

void video::ServiceIdChanger::HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid)
{
	// 如果此 PMT 的 service_id 有在映射表中，更改它。
	auto it = _service_id_map.find(pmt.service_id);
	if (it != _service_id_map.end())
	{
		pmt.service_id = it->second;
	}

	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pmt, source_pid));
}

void video::ServiceIdChanger::HandleSdtVersionChange(ts::BinaryTable const &table)
{
	ts::SDT sdt;
	sdt.deserialize(*_duck, table);
	ts::SDT backup_sdt{ sdt };
	for (auto &map_pair : _service_id_map)
	{
		auto it = sdt.services.find(map_pair.first);
		if (it != sdt.services.end())
		{
			sdt.services.erase(it);
		}
	}

	for (auto &map_pair : _service_id_map)
	{
		auto it = backup_sdt.services.find(map_pair.first);
		if (it != backup_sdt.services.end())
		{
			sdt.services[map_pair.second] = it->second;
		}
	}

	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, sdt));
}

void video::ServiceIdChanger::ChangeServiceIdInPat(ts::PAT &pat)
{
	// 备份字典。
	std::map<uint16_t, uint16_t> backup_pmts{ pat.pmts };

	// 遍历映射表，找出哪些键是 pat.pmts 里确实有的，将它们从 pat.pmts 中移除。
	for (auto &map_pair : _service_id_map)
	{
		auto pmt = pat.pmts.find(map_pair.first);
		if (pmt != pat.pmts.end())
		{
			pat.pmts.erase(pmt);
		}
	}

	for (auto &map_pair : _service_id_map)
	{
		uint16_t src_service_id = map_pair.first;
		uint16_t dst_service_id = map_pair.second;
		auto it = backup_pmts.find(src_service_id);
		if (it != backup_pmts.end())
		{
			uint16_t pmt_pid = it->second;
			pat.pmts[dst_service_id] = pmt_pid;
		}
	}
}

void video::ServiceIdChanger::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (_streams_pid_set[packet->getPID()])
	{
		SendPacketToEachConsumer(packet);
	}
}
