#include "tsduck/handler/TableHandler.h"

video::TableHandler::TableHandler()
{
	_duck = shared_ptr<ts::DuckContext>{ new ts::DuckContext{&ts::CerrReport::Instance()} };
	_demux = shared_ptr<ts::SectionDemux>{
		new ts::SectionDemux{
			*_duck,
			nullptr,
			nullptr,
			ts::PIDSet{}
		}
	};
	_default_listened_pid_set[0] = 1;
	_default_listened_pid_set[0x11] = 1;
	_demux->setPIDFilter(_default_listened_pid_set);
	_demux->setTableHandler(this);
}

void video::TableHandler::handleTable(ts::SectionDemux &demux, ts::BinaryTable const &table)
{
	switch (table.tableId())
	{
	case ts::TID_PAT:
		{
			HandlePAT(table);
			break;
		}
	case ts::TID_PMT:
		{
			HandlePMT(table);
			break;
		}
	case ts::TID_SDT_ACT:
		{
			HandleSDT(table);
			break;
		}
	}
}

void video::TableHandler::ResetListenedPids()
{
	// _default_listened_pid_set 在构造函数中被初始化，对需要的 PID 对应的位进行置位。
	_demux->setPIDFilter(_default_listened_pid_set);
}

void video::TableHandler::ListenOnPmtPids(ts::PAT const &pat)
{
	for (auto &pmt : pat.pmts)
	{
		_demux->addPID(pmt.second);
	}
}
