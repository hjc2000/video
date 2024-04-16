#include"ReencodeDotNetVideoStream.h"

using namespace std;
using namespace jc;
using namespace video;

void ReencodeDotNetVideoStream(DotNetStream *dotnet_video_stream)
{
	// 想要输出的视频信息
	VideoStreamInfoCollection output_video_stream_infos;
	output_video_stream_infos._frame_rate = AVRational{ 30,1 };
	output_video_stream_infos._height = 1920;
	output_video_stream_infos._width = 1080;
	output_video_stream_infos._pixel_format = AVPixelFormat::AV_PIX_FMT_YUV420P;

	// 想要输出的音频信息
	AudioStreamInfoCollection output_audio_stream_infos;
	output_audio_stream_infos._ch_layout = AVChannelLayoutExtension::GetDefaultChannelLayout(2);
	output_audio_stream_infos._sample_format = AVSampleFormat::AV_SAMPLE_FMT_FLTP;
	output_audio_stream_infos._sample_rate = 48000;

	// 编码封装管道
	shared_ptr<Stream> out_fs = FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<StreamOutputFormat> out_fmt_ctx{ new StreamOutputFormat{".ts",out_fs} };
	shared_ptr<SptsEncodeMux> spts_encode_mux{ new SptsEncodeMux{
		out_fmt_ctx,
		output_video_stream_infos,
		"hevc_amf",
		-1,
		output_audio_stream_infos,
		"aac"
	} };

	// 输入格式
	shared_ptr<DotNetStream> dotnet_video_stream_sp{ dotnet_video_stream, [](DotNetStream *p) {} };
	shared_ptr<InputFormat> input_format{ new InputFormat{dotnet_video_stream_sp} };
	AVStreamInfoCollection input_video_stream_infos = input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	AVStreamInfoCollection input_audio_stream_infos = input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);

	// 解码管道
	shared_ptr<ThreadDecoderPipe> video_decode_pipe{ new ThreadDecoderPipe{input_video_stream_infos} };
	shared_ptr<ThreadDecoderPipe> audio_decode_pipe{ new ThreadDecoderPipe{input_audio_stream_infos} };
	video_decode_pipe->FrameConsumerList().Add(spts_encode_mux->VideoEncodePipe());
	audio_decode_pipe->FrameConsumerList().Add(spts_encode_mux->AudioEncodePipe());

	PacketPump pump{ input_format };
	pump.PacketConsumerList().Add(video_decode_pipe);
	pump.PacketConsumerList().Add(audio_decode_pipe);
	CancellationTokenSource cancel_pump_source;
	TaskCompletionSignal pump_thread_exit{ false };
	std::thread([&]()
	{
		try
		{
			pump.Pump(cancel_pump_source.Token());
		}
		catch (std::exception &e)
		{
			cerr << e.what() << endl;
		}
		catch (...)
		{
			cerr << "发生未知异常" << endl;
		}

		cout << "线程退出" << endl;
		pump_thread_exit.SetResult();
	}).detach();

	cin.get();
	cancel_pump_source.Cancel();
	pump_thread_exit.Wait();
}
