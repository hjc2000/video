#pragma once
#include<AVCodecContextWrapper.h>
#include<AVError.h>
#include<AVStreamWrapper.h>
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
		#pragma region 选项
		/**
		 * @brief VideoFrameDisplayer 选项类
		*/
		class Options :public IVideoStreamInfoCollection
		{
		public:
			Options() {}

			Options(IVideoStreamInfoCollection &infos)
			{
				IVideoStreamInfoCollection::operator=(infos);
			}

			Options &operator=(IVideoStreamInfoCollection &infos)
			{
				IVideoStreamInfoCollection::operator=(infos);
				return *this;
			}

		public:
			int _x = 0;
			int _y = 0;
			int _width = 1280;
			int _height = 720;
			AVRational _time_base{ 1,90000 };
			AVRational _frame_rate{ 30,1 };
			AVPixelFormat _pix_format = AVPixelFormat::AV_PIX_FMT_YUV420P;
			std::string _window_title = "VideoFrameDisplayer";
			SDL_WindowFlags _flags = SDL_WindowFlags::SDL_WINDOW_SHOWN;

		public:
			#pragma region IVideoStreamInfoCollection 属性
			virtual int width() override
			{
				return _width;
			}
			virtual void set_width(int value) override
			{
				_width = value;
			}

			virtual int height() override
			{
				return _height;
			}
			virtual void set_height(int value) override
			{
				_height = value;
			}

			virtual AVPixelFormat pixel_format() override
			{
				return _pix_format;
			}
			virtual void set_pixel_format(AVPixelFormat value) override
			{
				_pix_format = value;
			}

			virtual AVRational time_base() override
			{
				return _time_base;
			}
			virtual void set_time_base(AVRational value) override
			{
				_time_base = value;
			}

			virtual AVRational frame_rate() override
			{
				return _frame_rate;
			}
			virtual void set_frame_rate(AVRational value) override
			{
				_frame_rate = value;
			}
			#pragma endregion
		};
		#pragma endregion

	public:
		VideoFramePlayer(Options options);

		~VideoFramePlayer()
		{
			Dispose();
			cout << "~VideoFramePlayer()" << endl;
		}

	private:
		atomic_bool _disposed = false;
	public:
		/// <summary>
		///		关闭播放器。
		///		* 本方法线程安全。
		/// </summary>
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_frame_queue.Dispose();
			_timer.Stop();
		}

		#pragma region 参考时钟
	public:
		shared_ptr<IRefTimer> RefTimer()
		{
			return _ref_timer;
		}

		/**
		 * @brief 设置参考时钟。
		 * - 传入非空指针则开启同步。本视频帧播放器会同步到此参考时钟。
		 * - 传入空指针可以关闭同步。
		 * - 可以随时传入空指针来关闭音视频同步。
		 *
		 * @param value
		*/
		void SetRefTimer(shared_ptr<IRefTimer> value)
		{
			lock_guard l(_ref_timer_lock);
			_ref_timer = value;
		}

		/**
		 * @brief 获取当前的参考时间。单位：毫秒。
		 * @return
		*/
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

	private:
		SDL_Timer _timer;
		shared_ptr<VideoFrameDisplayer> _displayer;
		VideoStreamInfoCollection _video_stream_infos{};
		jc::HysteresisBlockingQueue<AVFrameWrapper> _frame_queue{ 10 };
		AVRational _time_base;

	private:
		/// <summary>
		///		SDL_Timer 回调处理函数。
		///		需要在这里向显示器送入帧。 
		/// </summary>
		/// <returns></returns>
		uint32_t SDL_TimerCallbackHandler(uint32_t interval_in_milliseconds);

	public:
		/// <summary>
		///		暂停播放。
		///		* 本方法不会阻塞，可以在回调函数中使用。
		/// </summary>
		/// <param name="pause"></param>
		void Pause(bool pause) override
		{
			/* 这里使用的是原子量，且 _timer 的方法是线程安全的，_video_frame_infos 又不会在另一个线程中
			* 被修改 frame_interval_in_milliseconds 属性，所以不用竞争 _not_private_methods_lock。
			*/
			if (pause)
			{
				// 暂停播放
				_timer.StopNoWait();
				return;
			}

			// 开始播放
			uint32_t interval = _video_stream_infos.frame_interval_in_milliseconds<uint32_t>();
			cout << "开始播放，帧间隔为：" << interval << endl;
			_timer.Start(interval);
		}

		/**
		 * @brief 将帧送入队列。送入空指针冲洗队列。
		 * - 播放器内部队列满时本方法会阻塞，直到消费到小于阈值才会取消阻塞。
		 *
		 * @param frame 要被送入队列的帧
		*/
		void SendFrame(AVFrameWrapper *frame) override
		{
			/* TODO
			* 需要在这里添加阻塞。在队列中元素数量达到一定程度后，进行阻塞。
			*/
			if (!frame)
			{
				_frame_queue.Flush();
				return;
			}

			// 这里不需要加锁，因为这里用的本来就是加了锁的，线程安全的队列。
			// 除非以后添加了别的代码，导致本方法不再线程安全。
			_frame_queue.Enqueue(*frame);
			return;
		}
	};
}
