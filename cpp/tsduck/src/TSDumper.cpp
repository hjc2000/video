#include"TSDumper.h"

using namespace std;

void video::TSDumper::HandlePAT(ts::BinaryTable const &table)
{
	ts::PAT pat;
	pat.deserialize(*_duck, table);
	for (auto &it : pat.pmts)
	{
		_demux->addPID(it.second);
	}

	if (_dump_pat)
	{
		Dump(ToString(pat));
		Dump("\n");
	}

	_demux->resetPID(0);
}

void video::TSDumper::HandlePMT(ts::BinaryTable const &table)
{
	if (_dump_pmt)
	{
		ts::PMT pmt;
		pmt.deserialize(*_duck, table);
		Dump(ToString(pmt));
		Dump("\n");
		_demux->resetPID(table.sourcePID());
	}
}

void video::TSDumper::HandleSDT(ts::BinaryTable const &table)
{
	if (_dump_sdt)
	{
		ts::SDT sdt;
		sdt.deserialize(*_duck, table);
		Dump(ToString(sdt, *_duck));
		Dump("\n");
		_demux->resetPID(0x11);
	}
}

void video::TSDumper::SendPacket(ts::TSPacket *packet)
{
	if (_dump_packet)
	{
		Dump(ToString(*packet));
	}

	_demux->feedPacket(*packet);
}
