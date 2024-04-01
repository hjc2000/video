#pragma once
#include<DecoderPipe.h>
#include<ErrorCode.h>
#include<HysteresisBlockingPacketQueue.h>
#include<IPacketConsumer.h>
#include<IPlayer.h>
#include<PacketPump.h>
#include<VideoFramePlayer.h>
#include<include_ffmpeg.h>

namespace video
{
	class VideoPacketPlayer :
		public IPlayer,
		public IPacketConsumer
	{
		#pragma region 生命周期
	public:
		/**
		 * @brief
		 * @param x 窗口的横坐标
		 * @param y 窗口的纵坐标
		 * @param stream 想要播放的流。必须是视频流。
		*/
		VideoPacketPlayer(int x, int y, AVStreamWrapper &stream)
		{
			#pragma region 安装管道
			/* 管道的安装需要按照从下游到上游的顺序。因为管道的每一节都是有一个列表指向出口处的接收者，
			* 初始化管道需要将接收者添加到列表中。
			*/

			// 播放器，管道最下游
			_player = shared_ptr<VideoFramePlayer>{
				new VideoFramePlayer{
					x,
					y,
					stream,
					"VideoPacketPlayer",
					SDL_WindowFlags::SDL_WINDOW_SHOWN,
				}
			};

			_decoder_pipe = shared_ptr<DecoderPipe>{ new DecoderPipe{stream} };
			_decoder_pipe->AddFrameConsumer(_player);

			// 包队列其实不算管道。它应该类似水池，需要一个泵将包送入管道。
			_packet_queue = shared_ptr<HysteresisBlockingPacketQueue>{ new HysteresisBlockingPacketQueue{} };

			// 将包从队列送到管道解码器的泵
			_packet_pump = shared_ptr<PacketPump>{ new PacketPump{_packet_queue} };
			_packet_pump->AddPacketConsumer(_decoder_pipe);
			#pragma endregion

			// 创建后台解码线程。
			thread([&]()
			{
				DecodingThreadFunc();
			}).detach();
		}

		~VideoPacketPlayer()
		{
			Dispose();
			cout << "~VideoPacketPlayer" << endl;
		}

	private:
		std::atomic_bool _disposed = false;

	public:
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_decoding_thread_can_start.Dispose();
			_cancel_pump_source.Cancel();
			_decoder_pipe->Dispose();
			_packet_queue->Dispose();
			_player->Dispose();
			_thread_has_exited.Wait();
		}
		#pragma endregion

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

		void Pause(bool pause) override
		{
			if (pause)
			{
				// 暂停播放
				_player->Pause(true);
				return;
			}

			// 开始播放
			_decoding_thread_can_start.SetResult();
			_thread_has_exited.Reset();
			_player->Pause(false);
		}

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

	private:
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
		void DecodingThreadFunc()
		{
			_decoding_thread_can_start.Wait();
			auto token = _cancel_pump_source.Token();
			_packet_pump->Pump(token);
			_thread_has_exited.SetResult();
		}
	};
}