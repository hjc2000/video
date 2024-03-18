#pragma once
#include<AVCodecContextWrapper.h>
#include<AVStreamWrapper.h>
#include<AudioFramePlayer.h>
#include<CancellationTokenSource.h>
#include<DecoderPipe.h>
#include<HysteresisBlockingPacketQueue.h>
#include<HysteresisBlockingQueue.h>
#include<PacketPump.h>
#include<TaskCompletionSignal.h>
#include<atomic>
#include<semaphore>

namespace video
{
	/**
	 * @brief 在回调中向播放器送入音频包，播放器会解码，然后播放。
	*/
	class AudioPacketPlayer :
		public IPlayer,
		public IRefTimer,
		public IPacketConsumer
	{
		#pragma region 生命周期
	public:
		/**
		 * @brief 通过音频流初始化播放器。包括创建内部解码器等操作。
		 * @param stream
		*/
		AudioPacketPlayer(AVStreamWrapper &stream)
		{
			#pragma region 安装管道
			// 根据音频流构造音频帧播放器
			_player = shared_ptr<AudioFramePlayer>{
				new AudioFramePlayer{stream}
			};

			// 根据音频流创建解码器
			_decoder_pipe = unique_ptr<DecoderPipe>{ new DecoderPipe{stream} };
			_decoder_pipe->AddFrameConsumer(_player);

			_packet_queue = shared_ptr<HysteresisBlockingPacketQueue>{ new HysteresisBlockingPacketQueue{} };

			// 将包从队列送到管道解码器的泵
			_packet_pump = shared_ptr<PacketPump>{ new PacketPump{_packet_queue} };
			_packet_pump->AddPacketConsumer(_decoder_pipe);
			#pragma endregion

			// 解码线程
			thread([&]()
			{
				DecodingThreadFunc();
			}).detach();
		}

		~AudioPacketPlayer()
		{
			Dispose();
			cout << "~AudioPacketPlayer" << endl;
		}

	private:
		atomic_bool _disposed = false;

	public:
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_decoding_thread_can_start.Dispose();
			_pump_cancellation_token_source.Cancel();
			_decoder_pipe->Dispose();
			_player->Dispose();
			_packet_queue->Dispose();
			_decoding_thread_has_exited.Wait();
		}
		#pragma endregion

		#pragma region 管道
	private:
		shared_ptr<AudioFramePlayer> _player;
		shared_ptr<DecoderPipe> _decoder_pipe;
		shared_ptr<HysteresisBlockingPacketQueue> _packet_queue;

		/**
		 * @brief 用于取消 _packet_pump 的泵送。
		*/
		CancellationTokenSource _pump_cancellation_token_source;

		/**
		 * @brief 将包从 _packet_queue 泵送到 _decoder_pipe 中。
		*/
		shared_ptr<PacketPump> _packet_pump;
		#pragma endregion

	private:
		/**
		 * @brief 此任务完成，说明解码线程函数 DecodingThreadFunc 已经结束了，且不会再执行了，已经彻底退出了。
		 * - 初始时线程没有启动，所以为已完成。
		*/
		TaskCompletionSignal _decoding_thread_has_exited{ true };

		/**
		 * @brief 线程启动后会立刻等待此信号。时机成熟后将此信号设置为完成以让线程函数真正开始执行。
		*/
		TaskCompletionSignal _decoding_thread_can_start{ false };

		/**
		 * @brief 用于解码的线程函数。
		*/
		void DecodingThreadFunc()
		{
			_decoding_thread_can_start.Wait();
			auto token = _pump_cancellation_token_source.Token();
			_packet_pump->Pump(token);
			_decoding_thread_has_exited.SetResult();
		}

	public:
		int64_t RefTime() override
		{
			return _player->RefTime();
		}

		/**
		 * @brief 暂停或开始播放。
		 * - 此方法会启动解码线程。
		 *
		 * @param pause 传入 false 开始播放，传入 true 暂停。
		*/
		void Pause(bool pause) override
		{
			if (pause)
			{
				/* 暂停播放
				* 暂停帧播放器。
				* 解码线程在帧播放器的缓冲区达到上限后会阻塞，所以只要暂停帧播放器，让帧播放器
				* 不要消费，就可以达到暂停播放的目的。
				*/
				_player->Pause(true);
			}
			else
			{
				// 开始播放
				_decoding_thread_has_exited.Reset();
				_decoding_thread_can_start.SetResult();
				_player->Pause(false);
			}
		}

		/**
		 * @brief 向播放器送入包。
		 * - 当内部的包队列满时，此函数会被阻塞。
		 *
		 * @param packet 要送入播放器的包。送入空指针表示冲洗播放器。
		 *
		 * @exception InvalidOperationException 冲洗后如果再调用本方法会抛出异常。
		 * @exception ObjectDisposedException
		*/
		void SendPacket(AVPacketWrapper *packet) override
		{
			_packet_queue->SendPacket(packet);
		}
	};
}
