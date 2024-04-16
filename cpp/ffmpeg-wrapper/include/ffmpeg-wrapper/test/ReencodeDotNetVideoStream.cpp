#include"ReencodeDotNetVideoStream.h"

using namespace std;
using namespace jc;
using namespace video;

void ReencodeDotNetVideoStream(DotNetStream *dotnet_video_stream)
{
	shared_ptr<DotNetStream> dotnet_video_stream_sp{ dotnet_video_stream, [](DotNetStream *p) {} };
	shared_ptr<InputFormat> input_format{ new InputFormat{dotnet_video_stream_sp} };

	// 想要输出的视频信息
	VideoStreamInfoCollection video_stream_infos;
	video_stream_infos._frame_rate = AVRational{ 30,1 };
	video_stream_infos._height = 1920;
	video_stream_infos._width = 1080;
	video_stream_infos._pixel_format = AVPixelFormat::AV_PIX_FMT_YUV420P;
	video_stream_infos._time_base = AVRational{ 1,90000 };
	// 想要输出的音频信息
	AudioStreamInfoCollection audio_stream_infos;
	audio_stream_infos._ch_layout = AVChannelLayoutExtension::GetDefaultChannelLayout(2);
	audio_stream_infos._sample_format = AVSampleFormat::AV_SAMPLE_FMT_FLTP;
	audio_stream_infos._sample_rate = 48000;
	audio_stream_infos._time_base = AVRational{ 1,90000 };

	// 输出格式
	shared_ptr<Stream> out_fs = FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<StreamOutputFormat> out_fmt_ctx{ new StreamOutputFormat{".ts",out_fs} };
	shared_ptr<SptsEncodeMux> spts_encode_mux{ new SptsEncodeMux{
		out_fmt_ctx,
		video_stream_infos,
		"hevc_amf",
		-1,
		audio_stream_infos,
		"aac"
	} };

	//joined_input_format_demux_decoder->AddVideoFrameConsumer(spts_encode_mux->VideoEncodePipe());
	//joined_input_format_demux_decoder->AddAudioFrameConsumer(spts_encode_mux->AudioEncodePipe());

	//CancellationTokenSource cancel_pump_source;
	//TaskCompletionSignal pump_thread_exit{ false };
	//std::thread([&]()
	//{
	//	try
	//	{
	//		joined_input_format_demux_decoder->Pump(cancel_pump_source.Token());
	//	}
	//	catch (std::exception &e)
	//	{
	//		cerr << e.what() << endl;
	//	}
	//	catch (...)
	//	{
	//		cerr << "发生未知异常" << endl;
	//	}

	//	cout << "线程退出" << endl;
	//	pump_thread_exit.SetResult();
	//}).detach();

	//cin.get();
	//cancel_pump_source.Cancel();
	//pump_thread_exit.Wait();
}
