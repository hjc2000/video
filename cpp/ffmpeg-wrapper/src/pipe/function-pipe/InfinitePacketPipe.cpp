#include"InfinitePacketPipe.h"

using namespace video;
using namespace std;

void InfinitePacketPipe::SendPacket(AVPacketWrapper *packet)
{
	if (!packet)
	{
		// 结尾处添加边距取最大值，不能让下一个封装的时间戳重叠到上一个。
		_offset += std::max(_last_pts, _last_dts) + _last_packet_duration;
		_last_pts = 0;
		_last_dts = 0;
		_last_packet_duration = 0;
		_start_pts_dts_not_set = true;
		return;
	}

	// packet 不为空指针
	if (packet->time_base() != AVRational{ 1,90000 })
	{
		cout << CODE_POS_STR << "送进来的包的时间基必须是 " << AVRational{ 1,90000 } << endl;
	}

	CorrectStartTimeStamp(*packet);
	UpdateLastPts(packet->pts());
	UpdateLastDts(packet->dts());
	UpdateLastPacketDuration(packet->Duration());
	packet->set_pts(packet->pts() + _offset);
	packet->set_dts(packet->dts() + _offset);
	SendPacketToEachConsumer(packet);
}
