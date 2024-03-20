#include"TableVersionChangeHandler.h"

void video::TableVersionChangeHandler::HandlePAT(ts::BinaryTable const &table)
{
	if (_pat_version == table.version())
	{
		// PAT 版本没有发生变化
		_demux->resetPID(table.sourcePID());
		return;
	}

	// PAT 版本发生变化
	ts::PAT pat;
	pat.deserialize(*_duck, table);

	_pat_version = table.version();
	_pmt_versions.clear();
	_streams_pid_set.reset();
	ResetListenedPids();
	ListenOnPmtPids(pat);

	if (_on_before_handling_new_version_pat)
	{
		_on_before_handling_new_version_pat(_current_pat, pat);
	}

	_current_pat = pat;
	HandlePatVersionChange(pat);
	_demux->reset();
}

void video::TableVersionChangeHandler::HandlePMT(ts::BinaryTable const &table)
{
	uint16_t source_pid = table.sourcePID();
	uint8_t version = table.version();
	auto version_it = _pmt_versions.find(source_pid);
	if (version_it == _pmt_versions.end() || version_it->second != version)
	{
		// 版本字典里没有，或者版本字典里有，但是版本号不同，说明收到新的 PMT
		_pmt_versions[source_pid] = version;
		ts::PMT pmt;
		pmt.deserialize(*_duck, table);
		_streams_pid_set << pmt;
		HandlePmtVersionChange(pmt, source_pid);
	}

	_demux->resetPID(source_pid);
}

void video::TableVersionChangeHandler::HandleSDT(ts::BinaryTable const &table)
{
	if (_sdt_version == table.version())
	{
		// SDT 版本没有发生变化
		_demux->resetPID(table.sourcePID());
		return;
	}

	// SDT 版本发生变化
	_sdt_version = table.version();
	HandleSdtVersionChange(table);
	_demux->resetPID(table.sourcePID());
}
