#pragma once
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketSource.h>
#include<ffmpeg-wrapper/wrapper/AVPacketWrapper.h>
#include<jccpp/container/HysteresisBlockingQueue.h>

namespace video
{
	/// <summary>
	///		包队列。内部使用带有滞回特性的 HysteresisBlockingQueue
	/// </summary>
	class HysteresisBlockingPacketQueue :
		public IPacketConsumer,
		public IPacketSource,
		public IDisposable
	{
	public:
		void Dispose() override;

	private:
		jc::HysteresisBlockingQueue<AVPacketWrapper> _packet_queue{ 10 };
		std::atomic_bool _disposed = false;

	public:
		/// <summary>
		///		向队列送入包
		/// </summary>
		/// <param name="packet">送入空指针冲洗内部队列</param>
		void SendPacket(AVPacketWrapper *packet);

		/// <summary>
		///		读取包
		/// </summary>
		/// <param name="packet"></param>
		/// <returns></returns>
		int ReadPacket(AVPacketWrapper &packet);
	};
}