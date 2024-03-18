#include"AutoServiceIdChanger.h"

using namespace video;
using namespace std;

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
