#pragma once
#include<ITSPacketConsumer.h>
#include<TableHandler.h>
#include<TsDuckToString.h>
#include<fstream>
#include<string>
#include<tsTSPacket.h>

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

		void HandlePAT(ts::BinaryTable const &table) override;
		void HandlePMT(ts::BinaryTable const &table) override;
		void HandleSDT(ts::BinaryTable const &table) override;

		void Dump(std::string str);

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override;

		bool _dump_packet = true;
		bool _dump_pat = true;
		bool _dump_pmt = true;
		bool _dump_sdt = true;
	};
}