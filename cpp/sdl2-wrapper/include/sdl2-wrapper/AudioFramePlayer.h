#pragma once
#include<condition_variable>
#include<ffmpeg-wrapper/AVChannelLayoutExtension.h>
#include<ffmpeg-wrapper/container/HysteresisBlockingFrameQueue.h>
#include<ffmpeg-wrapper/pipe/SwrPipe.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/signal-source/SinSignalSource.h>
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include<ffmpeg-wrapper/wrapper/SwrContextWrapper.h>
#include<jccpp/TimeInterpolator.h>
#include<sdl2-wrapper/IRefTimer.h>
#include<sdl2-wrapper/SDL_DefaultAudioDevice.h>
#include<sdl2-wrapper/SDL_EventGetter.h>
#include<sdl2-wrapper/SDL_Initializer.h>

namespace video
{
	class AudioFramePlayer :
		public IDisposable,
		public IRefTimer,
		public IFrameConsumer
	{
	public:
		/// <summary>
		///		要求 infos 中的 time_base 属性必须是有效的，本对象要使用此属性。
		/// </summary>
		/// <param name="infos"></param>
		AudioFramePlayer(IAudioStreamInfoCollection &infos);
		~AudioFramePlayer();
		void Dispose() override;

	private:
		atomic_bool _disposed = false;
		atomic_bool _should_pause = false;
		AVFrameWrapper _swr_out_frame;
		shared_ptr<SDL_DefaultAudioDevice> _device;
		AVRational _time_base;
		shared_ptr<HysteresisBlockingFrameQueue> _frame_queue{ new HysteresisBlockingFrameQueue };
		shared_ptr<SwrPipe> _swr_pipe;
		jccpp::TimeInterpolator _time_interpolator;

		void AudioCallbackHandler(uint8_t *stream, int len);

	public:
		int64_t RefTime() override;

		/// <summary>
		///		控制暂停与播放。
		/// </summary>
		/// <param name="pause">传入 true 表示暂停，传入 false 开始播放</param>
		void Pause(bool pause);

		/// <summary>
		///		向播放器送入帧，也可以冲洗播放器。
		/// </summary>
		/// <param name="frame">送入播放器的帧。送入空指针表示冲洗播放器。</param>
		void SendFrame(AVFrameWrapper *frame) override;
	};
}
