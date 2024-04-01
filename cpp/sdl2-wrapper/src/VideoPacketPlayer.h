#pragma once
#include<DecoderPipe.h>
#include<ErrorCode.h>
#include<HysteresisBlockingPacketQueue.h>
#include<IDisposable.h>
#include<IPacketConsumer.h>
#include<PacketPump.h>
#include<VideoFramePlayer.h>
#include<include_ffmpeg.h>

namespace video
{
	class VideoPacketPlayer :
		public IDisposable,
		public IPacketConsumer
	{
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

	private:
		std::atomic_bool _disposed = false;

		/// <summary>
		///		解码线程创建后会立刻等待此信号，当时机成熟，解码线程可以开始执行了，
		///		就将此信号设置为已完成。
		/// </summary>
		TaskCompletionSignal _decoding_thread_can_start{ false };

		/// <summary>
		///		解码线程退出后设为已完成。
		/// </summary>
		TaskCompletionSignal _thread_has_exited{ true };

		/// <summary>
		///		用于解码的线程函数
		/// </summary>
		void DecodingThreadFunc();

	public:
		/// <summary>
		///		向播放器送入包。可以送入空指针，用来冲洗播放器。
		///		* 当内部的包队列满时，此函数会被阻塞。
		/// </summary>
		/// <param name="packet">要送入播放器的包。送入空指针表示冲洗播放器。</param>
		/// <exception cref="ObjectDisposedException"></exception>
		/// <exception cref="InvalidOperationException">冲洗后如果再调用本方法会抛出异常。</exception>
		void SendPacket(AVPacketWrapper *packet) override
		{
			_packet_queue->SendPacket(packet);
		}

		void Pause(bool pause);

		#pragma region 参考时钟
	public:
		shared_ptr<IRefTimer> RefTimer()
		{
			return _player->RefTimer();
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
			_player->SetRefTimer(value);
		}
		#pragma endregion

		#pragma region 管道
	private:
		shared_ptr<HysteresisBlockingPacketQueue> _packet_queue;

		/**
		 * @brief 用于取消 _packet_pump 的泵送。
		*/
		CancellationTokenSource _cancel_pump_source;

		/**
		 * @brief 将包从 _packet_queue 泵送到 _decoder_pipe 中。
		*/
		shared_ptr<PacketPump> _packet_pump;
		shared_ptr<DecoderPipe> _decoder_pipe;
		shared_ptr<VideoFramePlayer> _player;
		#pragma endregion
	};
}