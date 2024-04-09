#include"ffmpeg-wrapper/mux/SptsEncodeMux.h"
#include<iostream>

using namespace video;
using namespace std;

video::SptsEncodeMux::SptsEncodeMux(
	shared_ptr<OutputFormat> out_format,
	// 视频相关参数
	IVideoStreamInfoCollection &video_stream_infos,
	std::string video_codec_name,
	int64_t video_out_bitrate_in_bps,
	// 音频相关参数
	IAudioStreamInfoCollection &audio_stream_infos,
	std::string audio_codec_name
)
{
	_out_format = out_format;

	// 视频参数
	_video_stream_infos = video_stream_infos;
	_video_codec_name = video_codec_name;
	_video_out_bitrate_in_bps = video_out_bitrate_in_bps;

	// 音频参数
	_audio_stream_infos = audio_stream_infos;
	_audio_codec_name = audio_codec_name;

	// ts 必须使用 1/90000 时间基
	_video_stream_infos.set_time_base(AVRational{ 1, 90000 });
	_audio_stream_infos.set_time_base(AVRational{ 1, 90000 });

	InitVideoEncodePipe();
	InitAudioEncodePipe();
	WriteHeader();
}

void video::SptsEncodeMux::InitVideoEncodePipe()
{
	try
	{
		_video_encode_pipe = shared_ptr<SwsFpsEncoderPipe>{
			new SwsFpsEncoderPipe{
				_video_codec_name,
				_video_stream_infos,
				_out_format,
				_video_out_bitrate_in_bps,
			}
		};
	}
	catch (...)
	{
		cout << CODE_POS_STR
			<< std::format("找不到编码器 {}，现在尝试使用 libx265。", _video_codec_name)
			<< endl;

		_video_encode_pipe = shared_ptr<SwsFpsEncoderPipe>{
			new SwsFpsEncoderPipe{
				"libx265",
				_video_stream_infos,
				_out_format,
				_video_out_bitrate_in_bps,
			}
		};
	}
}

void video::SptsEncodeMux::InitAudioEncodePipe()
{
	_audio_encode_pipe = shared_ptr<SwrEncoderPipe>{
		new SwrEncoderPipe{
			_audio_codec_name.c_str(),
			_audio_stream_infos,
			_out_format,
		}
	};
}

void video::SptsEncodeMux::WriteHeader()
{
	AVDictionaryWrapper option_dic;
	option_dic.SetValueByKey("mpegts_flags", "resend_headers");
	option_dic.SetValueByKey("pat_period ", "1000");
	option_dic.SetValueByKey("pmt_period", "1000");
	option_dic.SetValueByKey("sdt_period", "1000");
	_out_format->WriteHeader(option_dic);
	_out_format->DumpFormat();
}

shared_ptr<IFrameConsumer> video::SptsEncodeMux::VideoEncodePipe()
{
	return _video_encode_pipe;
}

shared_ptr<IFrameConsumer> video::SptsEncodeMux::AudioEncodePipe()
{
	return _audio_encode_pipe;
}


#include<jccpp/container/List.h>
#include<ffmpeg-wrapper/format/input-format/InputFormat.h>
#include<jccpp/stream/FileStream.h>
#include<ffmpeg-wrapper/demux/JoinedInputFormatDemuxDecoder.h>
#include<ffmpeg-wrapper/format/output-format/StreamOutputFormat.h>

/// <summary>
///		测试函数
/// </summary>
void test_SptsEncodeMux()
{
	shared_ptr<Stream> out_fs = FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<StreamOutputFormat> out_fmt_ctx{ new StreamOutputFormat{".ts",out_fs} };

	VideoStreamInfoCollection out_video_stream_infos;
	out_video_stream_infos._frame_rate = AVRational{ 30,1 };
	out_video_stream_infos._width = 1920;
	out_video_stream_infos._height = 1080;
	out_video_stream_infos._pixel_format = AVPixelFormat::AV_PIX_FMT_YUV420P;
	out_video_stream_infos._time_base = AVRational{ 1,90000 };

	AudioStreamInfoCollection out_audio_stream_infos;
	out_audio_stream_infos._ch_layout = AVChannelLayoutExtension::get_2_0_layout_channel();
	out_audio_stream_infos._sample_format = AVSampleFormat::AV_SAMPLE_FMT_FLTP;
	out_audio_stream_infos._sample_rate = 48000;
	out_audio_stream_infos._time_base = AVRational{ 1,90000 };

	shared_ptr<SptsEncodeMux> spts_encode_mux{
		new SptsEncodeMux{
			out_fmt_ctx,
			out_video_stream_infos,
			"hevc_amf",
			-1,
			out_audio_stream_infos,
			"aac"
		}
	};

	Queue<std::string> file_queue;
	file_queue.Enqueue("fallen-down.ts");
	file_queue.Enqueue("Nightglow.mp4");
	file_queue.Enqueue("水龙吟.ts");
	file_queue.Enqueue("idol.mp4");
	file_queue.Enqueue("moon.mp4");
	file_queue.Enqueue("zf.ts");

	shared_ptr<JoinedInputFormatDemuxDecoder> joined_input_format_demux_decoder{ new JoinedInputFormatDemuxDecoder{} };
	joined_input_format_demux_decoder->AddVideoFrameConsumer(spts_encode_mux->VideoEncodePipe());
	joined_input_format_demux_decoder->AddAudioFrameConsumer(spts_encode_mux->AudioEncodePipe());
	joined_input_format_demux_decoder->_get_format_callback = [&]()->shared_ptr<InputFormat>
	{
		try
		{
			std::string file = file_queue.Dequeue();
			shared_ptr<InputFormat> in_fmt_ctx{ new InputFormat{ file } };
			in_fmt_ctx->DumpFormat();
			return in_fmt_ctx;
		}
		catch (std::exception &e)
		{
			cerr << e.what() << endl;
		}
		catch (...)
		{
			cerr << "发生未知异常" << endl;
		}

		return nullptr;
	};

	CancellationTokenSource cancel_pump_source;
	TaskCompletionSignal pump_thread_exit{ false };
	std::thread([&]()
	{
		try
		{
			joined_input_format_demux_decoder->Pump(cancel_pump_source.Token());
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
