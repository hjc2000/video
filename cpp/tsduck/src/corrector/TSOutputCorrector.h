#pragma once
#include<CCCorrector.h>
#include<TableRepeater.h>

namespace video
{
	/// <summary>
	///		输出校正器。组合了多个校正器，用来当作输出端口，对输出进行矫正，这样就不用每次
	///		都重复串联一大堆管道了。
	/// </summary>
	class TSOutputCorrector :
		public IPipeTsPacketSource,
		public ITSPacketConsumer
	{
	public:
		TSOutputCorrector();

	private:
		shared_ptr<CCCorrector> _cccorrect{ new CCCorrector{} };
		shared_ptr<TableRepeater> _repeater{ new TableRepeater{} };

	public:
		#pragma region 通过 IPipeTsPacketSource 继承
		void AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) override;
		bool RemovePacketComsumer(shared_ptr<ITSPacketConsumer> packet_comsumer) override;
		void ClearConsumers() override;
		#pragma endregion

		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;
	};
}