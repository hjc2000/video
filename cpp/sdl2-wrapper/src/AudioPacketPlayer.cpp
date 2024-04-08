#include"sdl2-wrapper/AudioPacketPlayer.h"

using namespace video;
using namespace std;

AudioPacketPlayer::AudioPacketPlayer(AVStreamWrapper &stream)
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

AudioPacketPlayer::~AudioPacketPlayer()
{
	Dispose();
	cout << "~AudioPacketPlayer" << endl;
}

void AudioPacketPlayer::Dispose()
{
	if (_disposed) return;
	_disposed = true;

	_decoding_thread_can_start.Dispose();
	_cancel_pump_source.Cancel();
	_decoder_pipe->Dispose();
	_player->Dispose();
	_packet_queue->Dispose();
	_decoding_thread_has_exited.Wait();
}

void AudioPacketPlayer::DecodingThreadFunc()
{
	_decoding_thread_can_start.Wait();
	_packet_pump->Pump(_cancel_pump_source.Token());
	_decoding_thread_has_exited.SetResult();
}

int64_t AudioPacketPlayer::RefTime()
{
	return _player->RefTime();
}

void AudioPacketPlayer::Pause(bool pause)
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

void AudioPacketPlayer::SendPacket(AVPacketWrapper *packet)
{
	_packet_queue->SendPacket(packet);
}
