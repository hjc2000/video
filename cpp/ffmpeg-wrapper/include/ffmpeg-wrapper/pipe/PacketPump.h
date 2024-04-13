#pragma once
#include<atomic>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/pipe/interface/IPipePacketSource.h>
#include<jccpp/CancellationTokenSource.h>
#include<jccpp/IDisposable.h>

namespace video
{
	/// <summary>
	///		从 IPacketSource 中读取包，送入 IPacketConsumer
	/// </summary>
	class PacketPump :
		public IPipePacketSource,
		public IDisposable
	{
		std::atomic_bool _disposed = false;
		shared_ptr<IPacketSource> _packet_source;
		List<shared_ptr<IPacketConsumer>> _consumer_list;

	public:
		PacketPump(shared_ptr<IPacketSource> packet_source) :
			_packet_source(packet_source)
		{

		}

		~PacketPump()
		{
			Dispose();
		}

		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;
		}

		List<shared_ptr<IPacketConsumer>> &PacketConsumerList() override
		{
			return _consumer_list;
		}

		/// <summary>
		///		每次读取包后，在将包送给消费者之前会回调。
		/// </summary>
		std::function<void(AVPacketWrapper *packet)> _on_before_send_packet_to_consumer;

		void Pump(shared_ptr<CancellationToken> cancellation_token)
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
	};
}