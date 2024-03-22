#include<AVIOContextWrapper.h>
#include<AVPacketPlayer.h>
#include<FileStream.h>
#include<InputFormat.h>
#include<PacketPump.h>

void video::test_AVPacketPlayer()
{
	auto fs = FileStream::Open("渡尘.mp4");
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
