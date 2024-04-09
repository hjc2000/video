#pragma once
#include<fstream>
#include<string>
#include<tsTSPacket.h>
#include<tsduck/TsDuckToString.h>
#include<tsduck/handler/TableHandler.h>
#include<tsduck/interface/ITSPacketConsumer.h>

namespace video
{
	class TSDumper :
		public ITSPacketConsumer,
		public TableHandler
	{
	public:
		TSDumper(std::string output_file_path);

	private:
		std::string _output_file_path;
		shared_ptr<std::fstream> _output_fs;

		uint64_t _total_packet_count = 0;
		std::map<uint16_t, uint64_t> _pid__packet_count_map;

		void HandlePAT(ts::BinaryTable const &table) override;
		void HandlePMT(ts::BinaryTable const &table) override;
		void HandleSDT(ts::BinaryTable const &table) override;

		void Dump(std::string str);

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;

		void DisplayStatisticalResults();

		bool _dump_packet = true;
		bool _dump_pat = true;
		bool _dump_pmt = true;
		bool _dump_sdt = true;
	};
}