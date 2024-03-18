#pragma once
#include<Exception.h>
#include<ITSPacketConsumer.h>
#include<PipeTsPacketSource.h>
#include<TableOperator.h>
#include<TableVersionChangeHandler.h>
#include<TsDuckToString.h>
#include<map>
#include<tsBinaryTable.h>
#include<tsDuckContext.h>
#include<tsSDT.h>
#include<tsSectionDemux.h>

namespace video
{
	class ServiceIdChanger :
		public ITsPacketConsumer,
		public PipeTsPacketSource,
		public TableVersionChangeHandler
	{
	public:
		ServiceIdChanger(std::map<uint16_t, uint16_t> service_id_map);

	private:
		std::map<uint16_t, uint16_t> _service_id_map;

		void HandlePatVersionChange(ts::PAT &pat) override;
		void HandlePmtVersionChange(ts::PMT &pmt, uint16_t source_pid) override;
		void HandleSdtVersionChange(ts::BinaryTable const &table) override;

		void ChangeServiceIdInPat(ts::PAT &pat);

	public:
		using ITsPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;
	};
}