#pragma once
#include<AVCodecContextWrapper.h>
#include<AVStreamWrapper.h>
#include<ErrorCode.h>
#include<HysteresisBlockingQueue.h>
#include<IDisposable.h>
#include<IFrameConsumer.h>
#include<IPlayer.h>
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
	using namespace video;

	class VideoFramePlayer :public IPlayer, public IFrameConsumer
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
		atomic_bool _disposed = false;
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
		void Pause(bool pause) override;

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
		void SetRefTimer(shared_ptr<IRefTimer> value)
		{
			lock_guard l(_ref_timer_lock);
			_ref_timer = value;
		}

		/// <summary>
		///		获取当前的参考时间。单位：毫秒。
		/// </summary>
		/// <returns></returns>
		int64_t RefTime()
		{
			lock_guard l(_ref_timer_lock);
			return _ref_timer->RefTime();
		}

	private:
		/**
		* @brief 因为可以随时用 SetRefTimer 方法将 _ref_timer 设置为空指针以
		* 取消音频同步，所以需要互斥锁，避免 _ref_timer 在使用过程中被设为空指针。
		*/
		mutex _ref_timer_lock;
		shared_ptr<IRefTimer> _ref_timer;
		#pragma endregion
	};
}
