#include"sdl2-wrapper/VideoPacketPlayer.h"

using namespace video;
using namespace std;

VideoPacketPlayer::VideoPacketPlayer(int x, int y, AVStreamWrapper &stream)
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

VideoPacketPlayer::~VideoPacketPlayer()
{
	Dispose();
	cout << "~VideoPacketPlayer" << endl;
}

void VideoPacketPlayer::Dispose()
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

void VideoPacketPlayer::DecodingThreadFunc()
{
	_decoding_thread_can_start.Wait();
	auto token = _cancel_pump_source.Token();
	_packet_pump->Pump(token);
	_thread_has_exited.SetResult();
}

void VideoPacketPlayer::SendPacket(AVPacketWrapper *packet)
{
	_packet_queue->SendPacket(packet);
}

void VideoPacketPlayer::Pause(bool pause)
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

shared_ptr<IRefTimer> VideoPacketPlayer::RefTimer()
{
	return _player->RefTimer();
}

void VideoPacketPlayer::SetRefTimer(shared_ptr<IRefTimer> value)
{
	_player->SetRefTimer(value);
}
