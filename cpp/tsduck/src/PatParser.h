#pragma once
#include<AutoChangeIdProgramMux.h>
#include<TSDumper.h>
#include<TSOutputCorrector.h>
#include<TSPacketToStream.h>
#include<TableOperator.h>
#include<TsDuckToString.h>
#include<iostream>
#include<memory>
#include<tsAES.h>
#include<tsBinaryTable.h>
#include<tsCyclingPacketizer.h>
#include<tsDuckContext.h>
#include<tsNIT.h>
#include<tsPES.h>
#include<tsSDT.h>
#include<tsSectionDemux.h>
#include<tsTSFile.h>

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
		shared_ptr<TSPacketToStream> _ts_packet_to_stream{ new TSPacketToStream{FileStream::CreateNewAnyway("out.ts")} };

		shared_ptr<TSDumper> _packet_dumper{ new TSDumper{} };
		shared_ptr<AutoChangeIdProgramMux> _auto_change_id_program_mux{ new AutoChangeIdProgramMux{} };
		shared_ptr<ITsPacketConsumer> _input_port;
		shared_ptr<ITsPacketConsumer> _input_port1;

	public:
		void SendPacket(ts::TSPacket *packet) override
		{
			_input_port->SendPacket(packet);
			_input_port1->SendPacket(packet);
		}
	};
}
