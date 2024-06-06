#pragma once
#include<atomic>
#include<base/task/CancellationTokenSource.h>
#include<ffmpeg-wrapper/container/HysteresisBlockingPacketQueue.h>
#include<ffmpeg-wrapper/pipe/PacketPump.h>
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include<ffmpeg-wrapper/wrapper/AVCodecContextWrapper.h>
#include<ffmpeg-wrapper/wrapper/AVStreamWrapper.h>
#include<jccpp/container/HysteresisBlockingQueue.h>
#include<jccpp/TaskCompletionSignal.h>
#include<sdl2-wrapper/AudioFramePlayer.h>
#include<semaphore>

namespace video
{
	/**
	 * @brief 在回调中向播放器送入音频包，播放器会解码，然后播放。
	*/
	class AudioPacketPlayer :
		public IDisposable,
		public IRefTimer,
		public IPacketConsumer
	{
	public:
		/// <summary>
		///		通过音频流初始化播放器。包括创建内部解码器等操作。
		/// </summary>
		/// <param name="stream"></param>
		AudioPacketPlayer(AVStreamWrapper &stream);
		~AudioPacketPlayer();
		void Dispose() override;

	private:
		atomic_bool _disposed = false;
		shared_ptr<AudioFramePlayer> _player;
		shared_ptr<IDecoderPipe> _decoder_pipe;
		shared_ptr<HysteresisBlockingPacketQueue> _packet_queue;
		base::CancellationTokenSource _cancel_pump_source;
		shared_ptr<PacketPump> _packet_pump;

		/// <summary>
		///		此任务完成，说明解码线程函数 DecodingThreadFunc 已经结束了，且不会再执行了，
		///		已经彻底退出了。
		///		初始时线程没有启动，所以为已完成。
		/// </summary>
		TaskCompletionSignal _decoding_thread_has_exited { true };

		/// <summary>
		///		线程启动后会立刻等待此信号。时机成熟后将此信号设置为完成以让线程函数真正开始执行。
		/// </summary>
		TaskCompletionSignal _decoding_thread_can_start { false };

		/// <summary>
		///		用于解码的线程函数。
		/// </summary>
		void DecodingThreadFunc();

	public:
		int64_t RefTime() override;

		/// <summary>
		///		暂停或开始播放。此方法会启动解码线程。
		/// </summary>
		/// <param name="pause">传入 false 开始播放，传入 true 暂停。</param>
		void Pause(bool pause);

		/// <summary>
		///		向播放器送入包。
		///		当内部的包队列满时，此函数会被阻塞。
		/// </summary>
		/// <param name="packet">要送入播放器的包。送入空指针表示冲洗播放器。</param>
		/// <exception cref="InvalidOperationException">冲洗后如果再调用本方法会抛出异常。</exception>
		/// <exception cref="ObjectDisposedException"></exception>
		void SendPacket(AVPacketWrapper *packet) override;
	};
}
