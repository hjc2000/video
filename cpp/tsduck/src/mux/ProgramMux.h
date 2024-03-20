#pragma once
#include<Exception.h>
#include<ITSPacketConsumer.h>
#include<PipeTsPacketSource.h>
#include<TableHandler.h>
#include<TableOperator.h>
#include<TsDuckToString.h>

namespace video
{
	class ProgramMux :
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

		void HandlePAT(ts::BinaryTable const &table) override;
		void HandlePMT(ts::BinaryTable const &table) override;

		/// <summary>
		///		输入端在 SDT 版本没有发生变化的情况下不要乱送。没送一次 SDT，本函数都会合成新的 SDT，
		///		然后递增版本号，然后输出给消费者。
		/// </summary>
		/// <param name="table"></param>
		void HandleSDT(ts::BinaryTable const &table) override;

		void UpdatePat();

	public:
		/// <summary>
		///		将要混合的 ts 的包送进来，包括 PAT 的包。
		/// </summary>
		/// <param name="packet"></param>
		void SendPacket(ts::TSPacket *packet) override;

		/// <summary>
		///		输入端 PAT 发生变化后，需要调用此方法通知本对象，这样才能移除过期的服务。
		/// </summary>
		/// <param name="old_pat"></param>
		void OnPatVersionChange(ts::PAT const &old_pat);
	};
}