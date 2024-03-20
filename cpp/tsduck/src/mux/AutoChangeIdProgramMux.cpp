#include"AutoChangeIdProgramMux.h"

using namespace video;
using namespace std;

video::AutoChangeIdProgramMux::AutoChangeIdProgramMux(
	std::map<uint16_t, uint16_t> const &preset_pid_map,
	std::map<uint16_t, uint16_t> const &preset_service_id_map
)
{
	_preset_pid_map = preset_pid_map;
	_preset_service_id_map = preset_service_id_map;
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
