#include"AutoChangeIdProgramMux.h"

using namespace video;
using namespace std;


#pragma region 内部类
class AutoChangeIdProgramMux::ProgramMux :
	public ITSPacketConsumer,
	public PipeTsPacketSource,
	public TableHandler
{
private:
	ts::PAT _pat;
	ts::SDT _sdt;
	uint8_t _pat_version = 0;
	uint8_t _sdt_version = 0;
	ts::PIDSet _streams_pid_set;

	void HandlePAT(ts::BinaryTable const &table) override
	{
		ts::PAT pat;
		pat.deserialize(*_duck, table);
		_pat << pat;
		UpdatePat();
	}

	void HandlePMT(ts::BinaryTable const &table) override
	{
		ts::PMT pmt;
		pmt.deserialize(*_duck, table);
		_streams_pid_set << pmt;
		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pmt, table.sourcePID()));
		_demux->resetPID(table.sourcePID());
	}

	/// <summary>
	///		输入端在 SDT 版本没有发生变化的情况下不要乱送。没送一次 SDT，本函数都会合成新的 SDT，
	///		然后递增版本号，然后输出给消费者。
	/// </summary>
	/// <param name="table"></param>
	void HandleSDT(ts::BinaryTable const &table) override
	{
		ts::SDT sdt;
		sdt.deserialize(*_duck, table);
		_sdt << sdt;
		_sdt.version = _sdt_version++;
		TableOperator::RemoveRedundantServicesFromSdt(_sdt, _pat);
		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, _sdt));
		_demux->resetPID(table.sourcePID());
	}

	void UpdatePat()
	{
		_pat.version = _pat_version++;
		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, _pat));
		_streams_pid_set.reset();
		ResetListenedPids();
		ListenOnPmtPids(_pat);
		_demux->reset();
	}

public:
	/// <summary>
	///		将要混合的 ts 的包送进来，包括 PAT 的包。
	/// </summary>
	/// <param name="packet"></param>
	void SendPacket(ts::TSPacket *packet) override
	{
		_demux->feedPacket(*packet);
		if (_streams_pid_set[packet->getPID()])
		{
			SendPacketToEachConsumer(packet);
		}
	}

	/// <summary>
	///		输入端 PAT 发生变化后，需要调用此方法通知本对象，这样才能移除过期的服务。
	/// </summary>
	/// <param name="old_pat"></param>
	void OnPatVersionChange(ts::PAT const &old_pat)
	{
		for (auto &it : old_pat.pmts)
		{
			// 移除旧版本 PAT 的服务。
			_pat.pmts.erase(it.first);
		}
	}
};
#pragma endregion







video::AutoChangeIdProgramMux::AutoChangeIdProgramMux(
	std::map<uint16_t, uint16_t> const &preset_pid_map,
	std::map<uint16_t, uint16_t> const &preset_service_id_map
)
{
	_preset_pid_map = preset_pid_map;
	_preset_service_id_map = preset_service_id_map;
	_program_mux = shared_ptr<ProgramMux>{ new ProgramMux{} };
}

shared_ptr<ITSPacketConsumer> video::AutoChangeIdProgramMux::GetNewInputPort()
{
	shared_ptr<AutoPidChanger> auto_pid_changer{ new AutoPidChanger{_pid_provider,_preset_pid_map} };
	auto_pid_changer->AddTsPacketConsumer(_program_mux);
	auto_pid_changer->_on_before_handling_new_version_pat = [&](ts::PAT const &old_pat, ts::PAT &new_pat)
	{
		_program_mux->OnPatVersionChange(old_pat);
	};

	shared_ptr<AutoServiceIdChanger> auto_service_id_changer{
		new AutoServiceIdChanger{
			_service_id_provider,
			_preset_service_id_map
		}
	};

	auto_service_id_changer->AddTsPacketConsumer(auto_pid_changer);
	return auto_service_id_changer;
}

void video::AutoChangeIdProgramMux::AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	_program_mux->AddTsPacketConsumer(packet_comsumer);
}

bool video::AutoChangeIdProgramMux::RemovePacketComsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	return _program_mux->RemovePacketComsumer(packet_comsumer);
}

void video::AutoChangeIdProgramMux::ClearConsumers()
{
	_program_mux->ClearConsumers();
}
