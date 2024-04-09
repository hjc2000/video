#pragma once
#include<jccpp/IDisposable.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameSource.h>
#include<jccpp/container/HysteresisBlockingQueue.h>

namespace video
{
	class HysteresisBlockingFrameQueue :
		public IFrameConsumer,
		public IFrameSource,
		public IDisposable
	{
	public:
		#pragma region 生命周期
	private:
		std::atomic_bool _disposed = false;

	public:
		void Dispose()
		{
			if (_disposed) return;
			_disposed = true;

			_frame_queue.Dispose();
		}
		#pragma endregion

	private:
		jc::HysteresisBlockingQueue<AVFrameWrapper> _frame_queue{ 10 };

	public:
		/**
		 * @brief 读取帧
		 * @param frame
		 * @return 成功返回 0，失败返回负数的错误代码。
		 * - 如果队列没被冲洗，永远返回 0. 队列中暂时没有帧会收到阻塞。
		 * - 如果队列被冲洗了，且读完所有帧了，则返回 ErrorCode::eof。
		*/
		int ReadFrame(AVFrameWrapper &frame) override;

		/**
		 * @brief 向帧队列送入帧。
		 * @param frame 要送入的帧。送入空指针冲洗队列。冲洗后不能再次送入帧了，否则会引发异常。
		 *
		 * @exception InvalidOperationException 冲洗后如果再调用本方法会抛出异常。
		 * @exception ObjectDisposedException Dispose 后再次调用本方法会抛出异常。
		*/
		void SendFrame(AVFrameWrapper *frame) override;
	};
}