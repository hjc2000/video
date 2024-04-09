#pragma once
#include<atomic>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/pipe/interface/PipePacketSource.h>
#include<jccpp/CancellationTokenSource.h>
#include<jccpp/IDisposable.h>

namespace video
{
	/**
	 * @brief 从 IPacketSource 中读取包，送入 IPacketConsumer
	*/
	class PacketPump :
		public PipePacketSource,
		public IDisposable
	{
		#pragma region 生命周期
	public:
		PacketPump(shared_ptr<IPacketSource> packet_source) :
			_packet_source(packet_source)
		{

		}

		~PacketPump()
		{
			Dispose();
		}

	private:
		std::atomic_bool _disposed = false;

	public:
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;
		}
		#pragma endregion

	public:
		/// <summary>
		///		每次读取包后，在将包送给消费者之前会回调。
		/// </summary>
		std::function<void(AVPacketWrapper *packet)> _on_before_send_packet_to_consumer;

		/**
		 * @brief 泵送。
		 * 将包从源中读出来，送给每一个消费者。要求包源在包结束时返回 ErrorCode::eof，
		 * 在读取成功时返回 0. 在其他错误时返回其他错误代码。但是返回其他错误代码会导致本函数
		 * 抛出异常。
		 *
		 * @param cancellation_token 用于取消泵送的令牌
		*/
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

	private:
		shared_ptr<IPacketSource> _packet_source;
	};
}