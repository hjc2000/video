#pragma once
#include<AudioPacketPlayer.h>
#include<HysteresisBlockingQueue.h>
#include<IDisposable.h>
#include<IPacketConsumer.h>
#include<VideoPacketPlayer.h>

namespace video
{
	/// <summary>
	///		同时播放一路流的音频包和一路流的视频包。
	/// </summary>
	class AVPacketPlayer :
		public IDisposable,
		public IPacketConsumer
	{
		#pragma region 生命周期
	public:
		/**
		 * @brief
		 * @param x 窗口横坐标
		 * @param y 窗口纵坐标
		 * @param video_stream 要播放的视频流
		 * @param audio_stream 要播放的音频流
		*/
		AVPacketPlayer(int x, int y, AVStreamWrapper &video_stream, AVStreamWrapper &audio_stream)
		{
			_audio_packet_player = shared_ptr<AudioPacketPlayer>{
				new AudioPacketPlayer{audio_stream}
			};

			_video_packet_player = shared_ptr<VideoPacketPlayer>{
				new VideoPacketPlayer{x,y,video_stream}
			};
			_video_packet_player->SetRefTimer(_audio_packet_player);

			_video_stream_index = video_stream.Index();
			_audio_stream_index = audio_stream.Index();
		}

		~AVPacketPlayer()
		{
			Dispose();
			cout << __func__ << endl;
		}

	private:
		atomic_bool _disposed = false;

	public:
		void Dispose() override
		{
			if (_disposed) return;
			_disposed = true;

			_audio_packet_player->Dispose();
			_video_packet_player->Dispose();
		}
		#pragma endregion

	private:
		shared_ptr<VideoPacketPlayer> _video_packet_player;
		shared_ptr<AudioPacketPlayer> _audio_packet_player;
		int _video_stream_index = 0;
		int _audio_stream_index = 0;

	public:
		void Pause(bool pause)
		{
			_audio_packet_player->Pause(pause);
			_video_packet_player->Pause(pause);
		}

		void SendPacket(AVPacketWrapper *packet) override
		{
			if (!packet)
			{
				_video_packet_player->SendPacket(nullptr);
				_audio_packet_player->SendPacket(nullptr);
				return;
			}

			if (packet->StreamIndex() == _video_stream_index)
			{
				_video_packet_player->SendPacket(packet);
			}
			else if (packet->StreamIndex() == _audio_stream_index)
			{
				_audio_packet_player->SendPacket(packet);
			}
		}
	};

	void test_AVPacketPlayer();
}
