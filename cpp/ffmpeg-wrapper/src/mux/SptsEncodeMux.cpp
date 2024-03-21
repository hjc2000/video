#include"SptsEncodeMux.h"
#include<iostream>

using namespace video;
using namespace std;

video::SptsEncodeMux::SptsEncodeMux(
	shared_ptr<OutputFormat> out_format,
	// 视频相关参数
	IVideoStreamInfoCollection &video_stream_infos,
	std::string video_codec_name,
	int64_t video_out_bitrate,
	// 音频相关参数
	IAudioStreamInfoCollection &audio_stream_infos,
	std::string audio_codec_name
)
{
	_out_format = out_format;

	// 视频参数
	_video_stream_infos = video_stream_infos;
	_video_codec_name = video_codec_name;
	_video_out_bitrate = video_out_bitrate;

	// 音频参数
	_audio_stream_infos = audio_stream_infos;
	_audio_codec_name = audio_codec_name;

	// ts 必须使用 1/90000 时间基
	_video_stream_infos.set_time_base(AVRational(1, 90000));
	_audio_stream_infos.set_time_base(AVRational(1, 90000));

	InitVideoEncodePipe();
	InitAudioEncodePipe();
}

void video::SptsEncodeMux::InitVideoEncodePipe()
{
	try
	{
		_video_encode_pipe = shared_ptr<SwsFpsEncoderPipe>{
			new SwsFpsEncoderPipe{
				_video_codec_name.c_str(),
				_video_stream_infos,
				_out_format,
				_video_out_bitrate,
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
				_video_out_bitrate,
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

shared_ptr<IFrameConsumer> video::SptsEncodeMux::GetVideoEncodePipe()
{
	return _video_encode_pipe;
}

shared_ptr<IFrameConsumer> video::SptsEncodeMux::GetAudioEncodePipe()
{
	return _audio_encode_pipe;
}


#include<List.h>
#include<InputFormatContext.h>
#include<InfiniteBestStreamDemuxDecoder.h>
#include<CustomOutputFormatContext.h>

/// <summary>
///		测试函数
/// </summary>
void test_SptsEncodeMux()
{
	List<std::string> format_list;
	format_list.Add("moon.mp4");
	format_list.Add("崩坏世界的歌姬.ts");
	format_list.Add("越权访问.mkv");

	// 节目解封装、解码管道
	int format_index = 0;
	auto get_format_callback = [&]()->shared_ptr<InputFormatContext>
	{
		shared_ptr<InputFormatContext> in_fmt_ctx{
			new InputFormatContext{ format_list[format_index] }
		};

		in_fmt_ctx->DumpFormat();
		format_index++;
		if (format_index >= format_list.Count())
		{
			format_index = 0;
		}

		return in_fmt_ctx;
	};

	shared_ptr<InfiniteBestStreamDemuxDecoder_old> best_stream_demux_decoder{
		new InfiniteBestStreamDemuxDecoder_old{get_format_callback}
	};

	// 准备输出格式
	shared_ptr<Stream> out_fs = FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<AVIOContextWrapper> out_io_ctx{ new AVIOContextWrapper{true, out_fs} };
	shared_ptr<CustomOutputFormatContext> out_fmt_ctx{
		new CustomOutputFormatContext{
			"mux_out.ts",
			out_io_ctx
		}
	};
}
