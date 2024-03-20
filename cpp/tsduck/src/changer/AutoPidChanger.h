#pragma once
#include<PidProvider.h>
#include<TableOperator.h>
#include<TableRepeater.h>
#include<TableVersionChangeHandler.h>

namespace video
{
	class AutoPidChanger :
		public TableVersionChangeHandler,
		public ITSPacketConsumer,
		public PipeTsPacketSource
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="pid_provider"></param>
		/// <param name="preset_pid_map">预设的映射表。如果有期望的自定义规则可以设置此参数。</param>
		AutoPidChanger(
			shared_ptr<PidProvider> pid_provider,
			std::map<uint16_t, uint16_t> const &preset_pid_map = std::map<uint16_t, uint16_t>{}
		);

	private:
		class PidChanger;
		shared_ptr<PidChanger> _pid_changer;
		shared_ptr<PidProvider> _pid_provider;
		std::map<uint16_t, uint16_t> _preset_pid_map;
		std::map<uint16_t, uint16_t> _final_pid_map;

		virtual void HandlePatVersionChange(ts::PAT &pat) override;
		virtual void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) override;

		#pragma region PipeTsPacketSource
	public:
		virtual void AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) override;
		virtual bool RemovePacketComsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) override;
		virtual void ClearConsumers() override;
		#pragma endregion

	public:
		void SendPacket(ts::TSPacket *packet) override;
	};
}
