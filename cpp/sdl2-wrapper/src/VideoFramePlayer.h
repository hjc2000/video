#pragma once
#include<AVCodecContextWrapper.h>
#include<AVStreamWrapper.h>
#include<ErrorCode.h>
#include<HysteresisBlockingQueue.h>
#include<IDisposable.h>
#include<IFrameConsumer.h>
#include<IRefTimer.h>
#include<SDL_Timer.h>
#include<SafeQueue.h>
#include<VideoFrameDisplayer.h>
#include<VideoStreamInfoCollection.h>
#include<atomic>
#include<include_ffmpeg.h>
#include<mutex>
#include<semaphore>

namespace video
{
	class VideoFramePlayer :
		public IFrameConsumer,
		public IDisposable
	{
	public:
		VideoFramePlayer(
			int x,
			int y,
			IVideoStreamInfoCollection &infos,
			std::string window_title,
			SDL_WindowFlags flags
		);

		~VideoFramePlayer();
		void Dispose() override;

	private:
		std::atomic_bool _disposed = false;
		SDL_Timer _timer;
		shared_ptr<VideoFrameDisplayer> _displayer;
		VideoStreamInfoCollection _video_stream_infos{};
		jc::HysteresisBlockingQueue<AVFrameWrapper> _frame_queue{ 10 };

		/// <summary>
		///		SDL_Timer 回调处理函数。
		///		需要在这里向显示器送入帧。 
		/// </summary>
		/// <returns></returns>
		uint32_t SDL_TimerCallbackHandler(uint32_t interval_in_milliseconds);

	public:
		/// <summary>
		///		暂停播放。
		///		本方法不会阻塞，可以在回调函数中使用。
		/// </summary>
		/// <param name="pause"></param>
		void Pause(bool pause);

		/// <summary>
		///		将帧送入队列。送入空指针冲洗队列。
		///		播放器内部队列满时本方法会阻塞，直到消费到小于阈值才会取消阻塞。
		/// </summary>
		/// <param name="frame">要被送入队列的帧</param>
		void SendFrame(AVFrameWrapper *frame) override;

		#pragma region 参考时钟
	public:
		shared_ptr<IRefTimer> RefTimer();

		/// <summary>
		///		设置参考时钟。
		///		- 传入非空指针则开启同步。本视频帧播放器会同步到此参考时钟。
		///		- 传入空指针可以关闭同步。
		///		- 可以随时传入空指针来关闭音视频同步。
		/// </summary>
		/// <param name="value"></param>
		void SetRefTimer(shared_ptr<IRefTimer> value);

		/// <summary>
		///		获取当前的参考时间。单位：毫秒。
		/// </summary>
		/// <returns></returns>
		int64_t RefTime();

	private:
		std::mutex _ref_timer_lock;
		shared_ptr<IRefTimer> _ref_timer;
		#pragma endregion
	};
}
