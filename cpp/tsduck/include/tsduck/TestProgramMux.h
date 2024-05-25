#pragma once
#include<jccpp/stream/FileStream.h>
#include<tsduck/corrector/TSOutputCorrector.h>
#include<tsduck/io/TSPacketStreamWriter.h>
#include<tsduck/mux/AutoChangeIdProgramMux.h>
#include<tsduck/TableOperator.h>
#include<tsduck/TSDumper.h>
#include<tsDuckContext.h>

namespace video
{
	using std::shared_ptr;
	using std::cout;
	using std::endl;

	class TestProgramMux :public ITSPacketConsumer
	{
	public:
		TestProgramMux()
		{
			_duck = shared_ptr<ts::DuckContext> { new ts::DuckContext { &ts::CerrReport::Instance() } };

			_output_corrector->AddTsPacketConsumer(_packet_dumper);
			_output_corrector->AddTsPacketConsumer(_ts_packet_to_stream);
			_auto_change_id_program_mux->AddTsPacketConsumer(_output_corrector);

			_input_port = _auto_change_id_program_mux->GetNewInputPort();
			_input_port1 = _auto_change_id_program_mux->GetNewInputPort();
		}

	private:
		shared_ptr<ts::DuckContext> _duck;

		shared_ptr<TSOutputCorrector> _output_corrector { new TSOutputCorrector { } };
		shared_ptr<TSPacketStreamWriter> _ts_packet_to_stream { new TSPacketStreamWriter { jccpp::FileStream::CreateNewAnyway("out.ts") } };

		shared_ptr<TSDumper> _packet_dumper { new TSDumper { "ts-dump.txt" } };
		shared_ptr<AutoChangeIdProgramMux> _auto_change_id_program_mux { new AutoChangeIdProgramMux { } };
		shared_ptr<ITSPacketConsumer> _input_port;
		shared_ptr<ITSPacketConsumer> _input_port1;

	public:
		using ITSPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override
		{
			_input_port->SendPacket(packet);
			_input_port1->SendPacket(packet);
		}
	};
}
