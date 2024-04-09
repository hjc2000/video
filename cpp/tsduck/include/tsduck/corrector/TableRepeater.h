#pragma once
#include<ITSPacketConsumer.h>
#include<PipeTsPacketSource.h>
#include<TableOperator.h>
#include<jccpp/Exception.h>
#include<tsDuckContext.h>
#include<tsSectionDemux.h>
#include<tsduck/handler/TableVersionChangeHandler.h>

namespace video
{
	class TableRepeater :
		public ITSPacketConsumer,
		public PipeTsPacketSource,
		public TableVersionChangeHandler
	{
	private:
		std::vector<ts::TSPacket> _pat_packets;
		std::vector<ts::TSPacket> _sdt_packets;
		std::vector<std::vector<ts::TSPacket>> _pmt_packet_vectors;

		/// <summary>
		///		上次送入包时的 PCR。
		/// </summary>
		uint64_t _pcr_at_last_send_pat_pmt = 0;
		int64_t _repeat_table_interval_in_milliseconds = 1000;

		void HandlePatVersionChange(ts::PAT &pat) override;
		void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) override;
		void HandleSdtVersionChange(ts::BinaryTable const &table) override;

		void SendTable();

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;

		/// <summary>
		///		重复发送表格的时间间隔。单位：毫秒。
		/// </summary>
		/// <returns></returns>
		int64_t RepeatPatPmtIntervalInMillisecond();
		void SetRepeatPatPmtIntervalInMillisecond(int64_t value);
	};
}