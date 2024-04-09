#include"tsduck/corrector/TSOutputCorrector.h"

using namespace video;
using namespace std;

video::TSOutputCorrector::TSOutputCorrector()
{
	_repeater->AddTsPacketConsumer(_cccorrect);
}

void video::TSOutputCorrector::AddTsPacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	_cccorrect->AddTsPacketConsumer(packet_comsumer);
}

bool video::TSOutputCorrector::RemovePacketConsumer(shared_ptr<ITSPacketConsumer> packet_comsumer)
{
	return _cccorrect->RemovePacketConsumer(packet_comsumer);
}

void video::TSOutputCorrector::ClearConsumers()
{
	_cccorrect->ClearConsumers();
}

void video::TSOutputCorrector::SendPacket(ts::TSPacket *packet)
{
	_repeater->SendPacket(packet);
}
