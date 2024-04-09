#include"tsduck/TSDumper.h"

using namespace video;
using namespace std;

TSDumper::TSDumper(std::string output_file_path)
{
	_output_fs = shared_ptr<std::fstream>{
		new std::fstream{
			output_file_path,
			std::ios_base::out | std::ios_base::trunc
		}
	};
}

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

void TSDumper::Dump(std::string str)
{
	//std::cout << str << std::endl;
	(*_output_fs) << str << std::endl;
}

void video::TSDumper::SendPacket(ts::TSPacket *packet)
{
	if (_dump_packet)
	{
		Dump(ToString(*packet));
	}

	_demux->feedPacket(*packet);
	_total_packet_count++;
	_pid__packet_count_map[packet->getPID()]++;
}

void video::TSDumper::DisplayStatisticalResults()
{
	cout << std::format("一共有 {} 个包", _total_packet_count) << endl;
	for (auto &it : _pid__packet_count_map)
	{
		uint16_t pid = it.first;
		uint64_t packet_count = it.second;
		cout << std::format(
			"pid = {}, packet_count = {}, rate = {}%",
			pid,
			packet_count,
			(double)packet_count * 100 / _total_packet_count
		) << endl;
	}
}
