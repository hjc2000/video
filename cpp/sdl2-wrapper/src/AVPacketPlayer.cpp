#include<ffmpeg-wrapper/wrapper/AVIOContextWrapper.h>
#include<sdl2-wrapper/AVPacketPlayer.h>
#include<jccpp/stream/FileStream.h>
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/pipe/PacketPump.h>

AVPacketPlayer::AVPacketPlayer(int x, int y, AVStreamWrapper &video_stream, AVStreamWrapper &audio_stream)
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

AVPacketPlayer::~AVPacketPlayer()
{
	Dispose();
	cout << __func__ << endl;
}

void AVPacketPlayer::Dispose()
{
	if (_disposed) return;
	_disposed = true;

	_audio_packet_player->Dispose();
	_video_packet_player->Dispose();
}

void AVPacketPlayer::Pause(bool pause)
{
	_audio_packet_player->Pause(pause);
	_video_packet_player->Pause(pause);
}

void AVPacketPlayer::SendPacket(AVPacketWrapper *packet)
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

void video::test_AVPacketPlayer()
{
	auto fs = FileStream::Open("fallen-down.ts");
	shared_ptr<AVIOContextWrapper> io_context{ new AVIOContextWrapper{ false, fs } };
	shared_ptr<InputFormat> in_fmt_ctx{ new InputFormat{ io_context } };
	in_fmt_ctx->DumpFormat();

	AVStreamWrapper best_audio_stream = in_fmt_ctx->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	AVStreamWrapper best_video_stream = in_fmt_ctx->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);

	shared_ptr<AVPacketPlayer> player{ new AVPacketPlayer{ 0, 70, best_video_stream, best_audio_stream } };
	AVPacketWrapper packet;

	CancellationTokenSource cancellation_token_source;
	auto cancellation_token = cancellation_token_source.Token();
	TaskCompletionSignal thread_has_exited{ false };

	PacketPump packet_pump{ in_fmt_ctx };
	packet_pump.AddPacketConsumer(player);
	thread([&]()
	{
		// 将包从封装泵送到播放器。
		packet_pump.Pump(cancellation_token);
		thread_has_exited.SetResult();
	}).detach();

	// 开始播放
	player->Pause(false);

	// 等待退出
	SDL_EventGetter event_getter;
	event_getter.WaitQuitEvent();
	cancellation_token_source.Cancel();
	player->Dispose();
	thread_has_exited.Wait();
}
