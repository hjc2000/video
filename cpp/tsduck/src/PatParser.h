#pragma once
#include<AutoChangeIdProgramMux.h>
#include<FileStream.h>
#include<TSDumper.h>
#include<TSOutputCorrector.h>
#include<TSPacketStreamWriter.h>
#include<TableOperator.h>
#include<tsDuckContext.h>

namespace video
{
	using std::shared_ptr;
	using std::cout;
	using std::endl;

	class PatParser :public ITsPacketConsumer
	{
	public:
		PatParser()
		{
			_duck = shared_ptr<ts::DuckContext>{ new ts::DuckContext{&ts::CerrReport::Instance()} };

			_output_corrector->AddTsPacketConsumer(_packet_dumper);
			_output_corrector->AddTsPacketConsumer(_ts_packet_to_stream);
			_auto_change_id_program_mux->AddTsPacketConsumer(_output_corrector);

			_input_port = _auto_change_id_program_mux->GetNewInputPort();
			_input_port1 = _auto_change_id_program_mux->GetNewInputPort();
		}

	private:
		shared_ptr<ts::DuckContext> _duck;

		shared_ptr<TSOutputCorrector> _output_corrector{ new TSOutputCorrector{} };
		shared_ptr<TSPacketStreamWriter> _ts_packet_to_stream{ new TSPacketStreamWriter{FileStream::CreateNewAnyway("out.ts")} };

		shared_ptr<TSDumper> _packet_dumper{ new TSDumper{} };
		shared_ptr<AutoChangeIdProgramMux> _auto_change_id_program_mux{ new AutoChangeIdProgramMux{} };
		shared_ptr<ITsPacketConsumer> _input_port;
		shared_ptr<ITsPacketConsumer> _input_port1;

	public:
		using ITsPacketConsumer::SendPacket;
		void SendPacket(ts::TSPacket *packet) override
		{
			_input_port->SendPacket(packet);
			_input_port1->SendPacket(packet);
		}
	};
}