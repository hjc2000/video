#include"tsduck/changer/AutoPidChanger.h"
#include<PidChanger.h>

using namespace std;
using namespace video;

video::AutoPidChanger::AutoPidChanger(
	shared_ptr<PidProvider> pid_provider,
	std::map<uint16_t, uint16_t> const &preset_pid_map
)
{
	_pid_provider = pid_provider;
	_preset_pid_map = preset_pid_map;
	for (auto &it : _preset_pid_map)
	{
		it.second = pid_provider->GetPid(it.second);
	}
}

void video::AutoPidChanger::HandlePatVersionChange(ts::PAT &pat)
{
	/* 遍历 _final_pid_map，找出 _preset_pid_map 中没有的，归还映射到的目标 PID。
	* 注意，映射到的目标 PID 是迭代器的 second 字段。
	*/
	for (auto &it : _final_pid_map)
	{
		if (_preset_pid_map.find(it.first) == _preset_pid_map.end())
		{
			_pid_provider->ReturnPid(it.second);
		}
	}

	_final_pid_map = _preset_pid_map;
	for (auto &pmt : pat.pmts)
	{
		if (_final_pid_map.find(pmt.second) == _final_pid_map.end())
		{
			_final_pid_map[pmt.second] = _pid_provider->GetPid(pmt.second);
		}
	}

	_pid_changer = shared_ptr<PidChanger>{ new PidChanger{_final_pid_map} };
	_pid_changer->AddTsPacketConsumerFromAnother(*this);
	_pid_changer->SendPacket(TableOperator::ToTsPacket(*_duck, pat));
}

void video::AutoPidChanger::HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid)
{
	for (auto &stream : pmt.streams)
	{
		if (_final_pid_map.find(stream.first) == _final_pid_map.end())
		{
			_final_pid_map[stream.first] = _pid_provider->GetPid(stream.first);
		}
	}

	_pid_changer->SetPidMap(_final_pid_map);
	_pid_changer->SendPacket(TableOperator::ToTsPacket(*_duck, pmt, source_pid));
}

#pragma region PipeTsPacketSource
void video::AutoPidChanger::AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	PipeTsPacketSource::AddTsPacketConsumer(packet_comsumer);
	if (_pid_changer)
	{
		_pid_changer->AddTsPacketConsumer(packet_comsumer);
	}
}

bool video::AutoPidChanger::RemovePacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	bool ret = false;
	ret |= PipeTsPacketSource::RemovePacketConsumer(packet_comsumer);
	if (_pid_changer)
	{
		ret |= _pid_changer->RemovePacketConsumer(packet_comsumer);
	}

	return ret;
}

void video::AutoPidChanger::ClearConsumers()
{
	PipeTsPacketSource::ClearConsumers();
	if (_pid_changer)
	{
		_pid_changer->ClearConsumers();
	}
}
#pragma endregion

void video::AutoPidChanger::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (_pid_changer)
	{
		if (_streams_pid_set[packet->getPID()])
		{
			_pid_changer->SendPacket(packet);
		}
		else if (packet->getPID() == 0x11)
		{
			_pid_changer->SendPacket(packet);
		}
	}
}
