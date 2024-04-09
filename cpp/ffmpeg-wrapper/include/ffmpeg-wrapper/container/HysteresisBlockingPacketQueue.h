#pragma once
#include<AVPacketWrapper.h>
#include<jccpp/container/HysteresisBlockingQueue.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>

namespace video
{
	/**
	 * @brief 包队列。内部使用带有滞回特性的 HysteresisBlockingQueue。
	*/
	class HysteresisBlockingPacketQueue :
		public IPacketConsumer,
		public IPacketSource,
		public IDisposable
	{
		#pragma region 生命周期
	private:
		std::atomic_bool _disposed = false;

	public:
		void Dispose()
		{
			if (_disposed) return;
			_disposed = true;

			_packet_queue.Dispose();
		}
		#pragma endregion

	private:
		jc::HysteresisBlockingQueue<AVPacketWrapper> _packet_queue{ 10 };

	public:
		/**
		 * @brief
		 * @param packet 送入空指针冲洗内部队列。
		 * @exception InvalidOperationException 冲洗后如果再调用本方法会抛出异常。
		 * @exception ObjectDisposedException
		*/
		void SendPacket(AVPacketWrapper *packet)
		{
			if (!packet)
			{
				_packet_queue.Flush();
				return;
			}

			_packet_queue.Enqueue(*packet);
		}

		/**
		 * @brief 读取包
		 * @param packet 用来接收包。
		 * @return 成功返回 0，失败返回 ErrorCode::eof。
		 *
		 * @exception ObjectDisposedException
		*/
		int ReadPacket(AVPacketWrapper &packet)
		{
			try
			{
				packet = _packet_queue.Dequeue();
				return 0;
			}
			catch (jc::InvalidOperationException &)
			{
				return (int)ErrorCode::eof;
			}
		}
	};
}