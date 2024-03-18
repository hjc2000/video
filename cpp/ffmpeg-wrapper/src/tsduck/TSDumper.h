#pragma once
#include<ITsPacketConsumer.h>
#include<TableHandler.h>
#include<TsDuckToString.h>
#include<fstream>
#include<tsTSPacket.h>

namespace video
{
	class TSDumper :
		public ITsPacketConsumer,
		public TableHandler
	{
	public:
		TSDumper() {}

	private:
		std::fstream _fs{ "ts-dump.txt", std::ios_base::out | std::ios_base::trunc };

		void HandlePAT(ts::BinaryTable const &table) override;
		void HandlePMT(ts::BinaryTable const &table) override;
		void HandleSDT(ts::BinaryTable const &table) override;

		void Dump(std::string str)
		{
			//std::cout << str << std::endl;
			_fs << str << std::endl;
		}

	public:
		using ITsPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;

		bool _dump_packet = true;
		bool _dump_pat = true;
		bool _dump_pmt = true;
		bool _dump_sdt = true;
	};
}