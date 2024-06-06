#pragma once
#include<base/task/CancellationTokenSource.h>
#include<ffmpeg-wrapper/base_include.h>
#include<ffmpeg-wrapper/container/HysteresisBlockingPacketQueue.h>
#include<ffmpeg-wrapper/ErrorCode.h>
#include<ffmpeg-wrapper/pipe/interface/IPacketConsumer.h>
#include<ffmpeg-wrapper/pipe/PacketPump.h>
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include<jccpp/IDisposable.h>
#include<sdl2-wrapper/VideoFramePlayer.h>

namespace video
{
	class VideoPacketPlayer :
		public IDisposable,
		public IPacketConsumer
	{
		std::atomic_bool _disposed = false;
		shared_ptr<HysteresisBlockingPacketQueue> _packet_queue;
		base::CancellationTokenSource _cancel_pump_source;
		shared_ptr<PacketPump> _packet_pump;
		shared_ptr<IDecoderPipe> _decoder_pipe;
		shared_ptr<VideoFramePlayer> _player;

		/// <summary>
		///		解码线程创建后会立刻等待此信号，当时机成熟，解码线程可以开始执行了，
		///		就将此信号设置为已完成。
		/// </summary>
		TaskCompletionSignal _decoding_thread_can_start { false };

		/// <summary>
		///		解码线程退出后设为已完成。
		/// </summary>
		TaskCompletionSignal _thread_has_exited { true };

		/// <summary>
		///		用于解码的线程函数
		/// </summary>
		void DecodingThreadFunc();

	public:
		/// <summary>
		///		
		/// </summary>
		/// <param name="x">窗口的横坐标</param>
		/// <param name="y">窗口的纵坐标</param>
		/// <param name="stream">想要播放的流。必须是视频流。</param>
		VideoPacketPlayer(int x, int y, AVStreamWrapper &stream);
		~VideoPacketPlayer();
		void Dispose() override;

		/// <summary>
		///		向播放器送入包。可以送入空指针，用来冲洗播放器。
		///		当内部的包队列满时，此函数会被阻塞。
		/// </summary>
		/// <param name="packet">要送入播放器的包。送入空指针表示冲洗播放器。</param>
		/// <exception cref="ObjectDisposedException"></exception>
		/// <exception cref="InvalidOperationException">冲洗后如果再调用本方法会抛出异常。</exception>
		void SendPacket(AVPacketWrapper *packet) override;

		void Pause(bool pause);

		shared_ptr<IRefTimer> RefTimer();

		/// <summary>
		///		设置参考时钟。
		///		- 传入非空指针则开启同步。本视频帧播放器会同步到此参考时钟。
		///		- 传入空指针可以关闭同步。
		///		- 可以随时传入空指针来关闭音视频同步。
		/// </summary>
		/// <param name="value"></param>
		void SetRefTimer(shared_ptr<IRefTimer> value);
	};
}