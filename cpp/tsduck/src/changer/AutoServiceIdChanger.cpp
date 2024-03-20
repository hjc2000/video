#include"AutoServiceIdChanger.h"

using namespace video;
using namespace std;

/**************************************************************************************/
/* 类型区 */
/**************************************************************************************/

class video::AutoServiceIdChanger::ServiceIdChanger :
	public ITsPacketConsumer,
	public PipeTsPacketSource,
	public TableVersionChangeHandler
{
public:
	ServiceIdChanger(std::map<uint16_t, uint16_t> service_id_map)
	{
		_service_id_map = service_id_map;
	}

private:
	std::map<uint16_t, uint16_t> _service_id_map;

	void HandlePatVersionChange(ts::PAT &pat) override
	{
		ChangeServiceIdInPat(pat);
		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pat));
	}

	void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) override
	{
		// 如果此 PMT 的 service_id 有在映射表中，更改它。
		auto it = _service_id_map.find(pmt.service_id);
		if (it != _service_id_map.end())
		{
			pmt.service_id = it->second;
		}

		SendPacketToEachConsumer(TableOperator::ToTsPacket(*_duck, pmt, source_pid));
	}

	void HandleSdtVersionChange(ts::BinaryTable const &table) override
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

	void ChangeServiceIdInPat(ts::PAT &pat)
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

public:
	using ITsPacketConsumer::SendPacket;
	void SendPacket(ts::TSPacket *packet) override
	{
		_demux->feedPacket(*packet);
		if (_streams_pid_set[packet->getPID()])
		{
			SendPacketToEachConsumer(packet);
		}
	}
};

video::AutoServiceIdChanger::AutoServiceIdChanger(
	shared_ptr<ServiceIdProvider> service_id_provider,
	std::map<uint16_t, uint16_t> const &preset_map
)
{
	_service_id_provider = service_id_provider;
	_preset_service_id_map = preset_map;
	for (auto &it : _preset_service_id_map)
	{
		// 尝试占用，没占用成功的会被自动分配。
		it.second = _service_id_provider->GetServiceId(it.second);
	}
}


/**************************************************************************************/
/* 函数区 */
/**************************************************************************************/

void video::AutoServiceIdChanger::HandlePatVersionChange(ts::PAT &pat)
{
	for (auto &it : _final_service_id_map)
	{
		if (_preset_service_id_map.find(it.first) == _preset_service_id_map.end())
		{
			_service_id_provider->ReturnServiceId(it.second);
		}
	}

	_final_service_id_map = _preset_service_id_map;
	// PAT 中可能会指示一些没有包含在当前映射表中的 service_id，这时需要添加映射规则。
	for (auto &it : pat.pmts)
	{
		if (_final_service_id_map.find(it.first) == _final_service_id_map.end())
		{
			// 此 service_id 没有在 _service_id_map 中。
			_final_service_id_map[it.first] = _service_id_provider->GetServiceId(it.first);
		}
	}

	// 得到最终的 _service_id_map 后，重新构造 _service_id_changer。
	_service_id_changer = shared_ptr<ServiceIdChanger>{ new ServiceIdChanger{_final_service_id_map} };
	_service_id_changer->AddTsPacketConsumerFromAnother(*this);
	_service_id_changer->SendPacket(TableOperator::ToTsPacket(*_duck, pat));
}

void video::AutoServiceIdChanger::SendPacket(ts::TSPacket *packet)
{
	_demux->feedPacket(*packet);
	if (_service_id_changer)
	{
		if (packet->getPID() == 0)
		{

		}
		else
		{
			_service_id_changer->SendPacket(packet);
		}
	}
}
