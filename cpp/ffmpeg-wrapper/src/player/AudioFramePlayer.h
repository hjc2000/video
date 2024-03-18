#pragma once
#include<AVChannelLayoutExtension.h>
#include<AVFrameWrapper.h>
#include<HysteresisBlockingFrameQueue.h>
#include<IFrameConsumer.h>
#include<IPlayer.h>
#include<IRefTimer.h>
#include<SDL_DefaultAudioDevice.h>
#include<SDL_EventGetter.h>
#include<SDL_Util.h>
#include<SinSignalSource.h>
#include<SwrContextWrapper.h>
#include<SwrPipe.h>
#include<TimeInterpolator.h>
#include<condition_variable>

namespace video
{
	using namespace video;

	/**
	 * @brief 本类线程安全。
	*/
	class AudioFramePlayer :
		public IPlayer,
		public IRefTimer,
		public IFrameConsumer
	{
	public:
		/**
		* @brief 要求 infos 中的 time_base 属性必须是有效的，本对象要使用此属性。
		* @param infos
		*/
		AudioFramePlayer(IAudioStreamInfoCollection &infos)
		{
			_time_base = infos.time_base();
			_device = shared_ptr<SDL_DefaultAudioDevice>{ new SDL_DefaultAudioDevice{} };
			cout << _device->ToString() << endl;

			_device->_audio_callback = [&](uint8_t *stream, int len)
			{
				AudioCallbackHandler(stream, len);
			};

			_swr_pipe = shared_ptr<SwrPipe>{ new SwrPipe {*_device} };
			_swr_pipe->AddFrameConsumer(_frame_queue);
		}

		~AudioFramePlayer()
		{
			Dispose();
			cout << "~AudioFramePlayer()" << endl;
		}

		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_frame_queue->Dispose();
			_device->Dispose();
		}

	private:
		atomic_bool _disposed = false;
		atomic_bool _should_pause = false;
		AVFrameWrapper _swr_out_frame;
		shared_ptr<SDL_DefaultAudioDevice> _device;
		AVRational _time_base;
		shared_ptr<HysteresisBlockingFrameQueue> _frame_queue{ new HysteresisBlockingFrameQueue };
		shared_ptr<SwrPipe> _swr_pipe;

		void AudioCallbackHandler(uint8_t *stream, int len)
		{
			if (_should_pause || _disposed)
			{
				return;
			}

			// 尝试从重采样器中读取帧
			int ret = _frame_queue->ReadFrame(_swr_out_frame);
			if (ret)
			{
				// 发生了未知错误，或者发生了 eof，即播放器已经被冲洗了。
				/* 冲洗播放器的方法是调用本播放器对象的 send_frame 方法时传入空指针，
				 * 这将冲洗重采样器。
				*/
				Pause(true);
				cout << "发生了错误或到达文件尾，AudioFramePlayer 停止播放" << endl;
				return;
			}

			// 读取成功
			_swr_out_frame.CopyAudioDataToBuffer(stream, len);
			_time_interpolator.SyncTime(_swr_out_frame.PtsToMilliseconds().count());
		}

	private:
		TimeInterpolator _time_interpolator;

	public:
		int64_t RefTime() override
		{
			return _time_interpolator.InterpolatedTimeInMilliseconds();
		}

	public:
		/**
		 * @brief 控制暂停与播放。
		 * @param pause 传入 true 表示暂停，传入 false 开始播放
		*/
		void Pause(bool pause) override
		{
			/* 这里没什么要加锁的，设置标志位而已。而且标志位还用了原子量。_device->Pause
			* 也是线程安全的。
			*/
			if (pause)
			{
				// 暂停播放
				_should_pause = true;
				_device->Pause(true);
				_time_interpolator.Pause();
			}
			else
			{
				// 启动播放（第一次启动或暂停后再次启动）
				_should_pause = false;
				_device->Pause(false);
			}
		}

		/**
		 * @brief 向播放器送入帧，也可以冲洗播放器。
		 * * 如果缓冲区快爆了，调用本方法会阻塞，直到底层的音频回调消耗一定数量的采样点。调用 Close
		 * 方法会取消这种阻塞。因为播放器都要关闭了，不能还有一个线程在那阻塞着。
		 *
		 * * 因为播放器内部有一个重采样器，有缓冲区，所以调用本方法后 frame 内的数据会被复制，
		 * 所以 frame 可以去接收别的帧，不会影响到重采样和播放。
		 *
		 * * 第一次送入帧时，本播放器会根据 frame 的 pts 属性，并根据本播放器的时间基，
		 * 计算出此 frame 的时间，然后作为音频的起始时间。注意，在这个过程中，frame 的
		 * 时间基会被设置成本播放器的时间基。
		 *
		 * @param frame 送入播放器的帧。送入空指针表示冲洗播放器。
		 *
		 * @exception SendFrameException
		*/
		void SendFrame(AVFrameWrapper *frame) override;
	};
}
