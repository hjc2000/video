#pragma once
#include<ServiceIdProvider.h>
#include<TableRepeater.h>
#include<TableVersionChangeHandler.h>

namespace video
{
	/// <summary>
	///		自动让 service_id 不冲突。发生冲突会改变 service_id，没有冲突就保持原来的值。
	/// </summary>
	class AutoServiceIdChanger :
		public TableVersionChangeHandler,
		public ITsPacketConsumer,
		public PipeTsPacketSource
	{
	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="service_id_provider"></param>
		/// <param name="preset_map">会尽量使用此映射表，如果有冲突，就使用自动分配的值。</param>
		AutoServiceIdChanger(
			shared_ptr<ServiceIdProvider> service_id_provider,
			std::map<uint16_t, uint16_t> const &preset_map = std::map<uint16_t, uint16_t>{}
		);

	private:
		class ServiceIdChanger;
		shared_ptr<ServiceIdChanger> _service_id_changer;
		shared_ptr<ServiceIdProvider> _service_id_provider;
		std::map<uint16_t, uint16_t> _preset_service_id_map;
		std::map<uint16_t, uint16_t> _final_service_id_map;

		void HandlePatVersionChange(ts::PAT &pat) override;

	public:
		void SendPacket(ts::TSPacket *packet) override;
	};
}