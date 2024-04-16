#include"PacketPump.h"

using namespace std;
using namespace video;

void video::PacketPump::Pump(shared_ptr<CancellationToken> cancellation_token)
{
	AVPacketWrapper packet;
	while (!_disposed)
	{
		if (cancellation_token->IsCancellationRequested())
		{
			return;
		}

		int ret = _packet_source->ReadPacket(packet);
		switch (ret)
		{
		case 0:
			{
				if (_on_before_send_packet_to_consumer)
				{
					_on_before_send_packet_to_consumer(&packet);
				}

				SendPacketToEachConsumer(&packet);
				break;
			}
		case (int)ErrorCode::eof:
			{
				cout << CODE_POS_STR << "遇到文件尾" << endl;
				if (_on_before_send_packet_to_consumer)
				{
					_on_before_send_packet_to_consumer(nullptr);
				}

				SendPacketToEachConsumer(nullptr);
				return;
			}
		default:
			{
				cout << CODE_POS_STR << "read_packet 返回了未知错误代码。" << endl;
				throw jc::Exception();
			}
		}
	}
}
