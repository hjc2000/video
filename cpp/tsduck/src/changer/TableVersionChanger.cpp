#include"TableVersionChanger.h"

using namespace video;
using namespace std;

void video::TableVersionChanger::SendPacket(ts::TSPacket *packet)
{
}

ITSPacketSource::ReadPacketResult video::TableVersionChanger::ReadPacket(ts::TSPacket &packet)
{
	return ITSPacketSource::ReadPacketResult();
}
