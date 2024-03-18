#include"MptsToSpts.h"

using namespace std;
using namespace video;

video::MptsToSpts::MptsToSpts(uint16_t service_id)
{
	_service_id = service_id;
}

void video::MptsToSpts::HandlePAT(ts::BinaryTable const &table)
{
	ts::PAT pat;
	pat.deserialize(*_duck, table);
	_pmt_pid_set = ts::PIDSet{};
	for (auto &it : pat.pmts)
	{
		_pmt_pid_set[it.second] = 1;
	}

	ts::PAT backup_pat{ pat };
	pat.pmts.clear();
	auto pmt_pair = backup_pat.pmts.find(_service_id);
	if (pmt_pair != backup_pat.pmts.end())
	{
		pat.pmts[pmt_pair->first] = pmt_pair->second;
		_pmt_pid = pmt_pair->second;
	}

	std::vector<ts::TSPacket> pat_packets = TableOperator::ToTsPacket(*_duck, pat);
	SendPacketToEachConsumer(pat_packets);
}

void video::MptsToSpts::HandleSDT(ts::BinaryTable const &table)
{
	ts::SDT sdt;
	sdt.deserialize(*_duck, table);

	ts::SDT backup_sdt{ sdt };
	sdt.services.clear();
	auto it = backup_sdt.services.find(_service_id);
	if (it != backup_sdt.services.end())
	{
		sdt.services[it->first] = it->second;
	}

	std::vector<ts::TSPacket> sdt_packets = TableOperator::ToTsPacket(*_duck, sdt);
	SendPacketToEachConsumer(sdt_packets);
}

void video::MptsToSpts::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	uint16_t pid = packet->getPID();
	if (_pmt_pid_set[pid] && pid != _pmt_pid)
	{
		// 拦截多余的 PMT
	}
	else if (pid == 0x11)
	{
		// 拦截多余的 SDT
	}
	else
	{
		SendPacketToEachConsumer(packet);
	}
}
