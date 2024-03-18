#include"ProgramMux.h"

using namespace video;
using namespace std;

void video::ProgramMux::HandlePAT(ts::BinaryTable const &table)
{
	ts::PAT pat;
	pat.deserialize(*_duck, table);
	_pat << pat;
	UpdatePat();
}

void video::ProgramMux::HandlePMT(ts::BinaryTable const &table)
{
	ts::PMT pmt;
	pmt.deserialize(*_duck, table);
	_streams_pid_set << pmt;
	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pmt, table.sourcePID()));
	_demux->resetPID(table.sourcePID());
}

void video::ProgramMux::HandleSDT(ts::BinaryTable const &table)
{
	ts::SDT sdt;
	sdt.deserialize(*_duck, table);
	_sdt << sdt;
	_sdt.version = _sdt_version++;
	TableOperator::RemoveRedundantServicesFromSdt(_sdt, _pat);
	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, _sdt));
	_demux->resetPID(table.sourcePID());
}

void video::ProgramMux::UpdatePat()
{
	_pat.version = _pat_version++;
	SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, _pat));
	_streams_pid_set.reset();
	ResetListenedPmtPids(_pat);
	_demux->reset();
}

void video::ProgramMux::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (_streams_pid_set[packet->getPID()])
	{
		SendPacketToEachConsumer(packet);
	}
}

void video::ProgramMux::OnPatVersionChange(ts::PAT const &old_pat)
{
	for (auto &it : old_pat.pmts)
	{
		// 移除旧版本 PAT 的服务。
		_pat.pmts.erase(it.first);
	}
}
