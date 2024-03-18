#include"ProgramEncoderMuxer.h"
#include<CustomOutputFormatContext.h>
#include<FileStream.h>
#include<InfiniteBestStreamDemuxDecoder.h>

using namespace video;

shared_ptr<MpegtsVideoStreamEncodeMuxPipe> video::ProgramEncoderMuxer::AddVideoStream(
	IVideoStreamInfoCollection &out_stream_infos,
	std::string codec_name,
	int64_t out_bittare,
	int out_pid,
	bool display_frame
)
{
	shared_ptr<MpegtsVideoStreamEncodeMuxPipe> pipe{
		new MpegtsVideoStreamEncodeMuxPipe{
			_out_fmt_ctx,
			_program,
			out_stream_infos,
			codec_name,
			out_bittare,
			out_pid,
			display_frame
		}
	};

	return pipe;
}

shared_ptr<MpegtsAudioStreamEncodeMuxPipe> video::ProgramEncoderMuxer::AddAudioStream(
	IAudioStreamInfoCollection &out_stream_infos,
	std::string codec_name,
	int out_pid
)
{
	shared_ptr<MpegtsAudioStreamEncodeMuxPipe> pipe{
		new MpegtsAudioStreamEncodeMuxPipe{
			_out_fmt_ctx,
			_program,
			out_stream_infos,
			codec_name,
			out_pid,
		}
	};

	return pipe;
}

void video::ProgramEncoderMuxer::AddMultiStream(
	shared_ptr<IMultiStreamPipeFrameSource> multi_source,
	std::string video_codec_name,
	std::string audio_codec_name,
	int out_audio_sample_rate,
	bool display_video_frame
)
{
	for (AVStreamInfoCollection &stream : multi_source->StreamList())
	{
		switch (stream._media_type)
		{
		case AVMediaType::AVMEDIA_TYPE_VIDEO:
			{
				shared_ptr<MpegtsVideoStreamEncodeMuxPipe> encode_pipe = AddVideoStream(
					stream,
					video_codec_name,
					-1,
					-1,
					display_video_frame
				);

				shared_ptr<IPipeFrameSource> pipe_frame_source = multi_source->GetPipeFrameSourceByStreamIndex(stream._stream_index);
				if (pipe_frame_source)
				{
					pipe_frame_source->AddFrameConsumer(encode_pipe);
				}
				else
				{
					cout << CODE_POS_STR << "在 map 中找不到解码管道" << endl;
				}

				break;
			}
		case AVMediaType::AVMEDIA_TYPE_AUDIO:
			{
				shared_ptr<MpegtsAudioStreamEncodeMuxPipe> encode_pipe;
				if (out_audio_sample_rate > 0)
				{
					AVStreamInfoCollection replace_stream = stream;
					replace_stream.set_sample_rate(out_audio_sample_rate);

					encode_pipe = AddAudioStream(
						replace_stream,
						audio_codec_name,
						-1
					);
				}
				else
				{
					encode_pipe = AddAudioStream(stream, audio_codec_name, -1);
				}

				shared_ptr<IPipeFrameSource> pipe_frame_source =
					multi_source->GetPipeFrameSourceByStreamIndex(stream._stream_index);

				if (pipe_frame_source)
				{
					pipe_frame_source->AddFrameConsumer(encode_pipe);
				}
				else
				{
					cout << CODE_POS_STR << "在 map 中找不到解码管道" << endl;
				}

				break;
			}
		default:
			{
				cout << CODE_POS_STR << "暂不支持该流的媒体类型。" << endl;
			}
		}
	}
}

void video::ProgramEncoderMuxer::WriteHeader()
{
	AVDictionaryWrapper option_dic;
	//option_dic.set("movflags", "frag_keyframe+empty_moov");

	/* 下面 3 条让 AVFormatContext 在封装的时候每隔一段时间重复 PAT 和 PMT 表。
	* 重复间隔短一点有助于接收端在任意时刻开始接收后快速启播。
	*/
	option_dic.SetValueByKey("mpegts_flags", "resend_headers");
	option_dic.SetValueByKey("sdt_period", "1000");
	option_dic.SetValueByKey("pat_period ", "1000");
	_out_fmt_ctx->WriteHeader(option_dic);
	_out_fmt_ctx->DumpFormat();
}

void video::test_ProgramEncoderMuxer()
{
	std::filesystem::current_path(Predefine_ResourceDir);
	List<std::string> format_list;
	format_list.Add("moon.mp4");
	format_list.Add("崩坏世界的歌姬.ts");
	format_list.Add("越权访问.mkv");

	// 节目解封装、解码管道
	int format_index = 0;
	auto get_format_callback = [&]()->shared_ptr<InputFormatContext>
	{
		shared_ptr<InputFormatContext> in_fmt_ctx{ new InputFormatContext{ format_list[format_index] } };
		in_fmt_ctx->dump_format();
		format_index++;
		if (format_index >= format_list.Count())
		{
			format_index = 0;
		}

		return in_fmt_ctx;
	};
	shared_ptr<InfiniteBestStreamDemuxDecoder> best_stream_demux_decoder{ new InfiniteBestStreamDemuxDecoder{get_format_callback} };

	// 节目编码、封装管道
	shared_ptr<Stream> out_fs = FileStream::CreateNewAnyway("mux_out.ts");
	shared_ptr<AVIOContextWrapper> out_io_ctx{ new AVIOContextWrapper{true, out_fs} };
	shared_ptr<CustomOutputFormatContext> out_fmt_ctx{ new CustomOutputFormatContext{"mux_out.ts", out_io_ctx} };
	shared_ptr<ProgramEncoderMuxer> program_encode_muxer{ new ProgramEncoderMuxer{out_fmt_ctx} };
	program_encode_muxer->AddMultiStream(
		best_stream_demux_decoder,
		"hevc_amf",
		"aac",
		44100,
		true
	);

	// 开始数据传输
	program_encode_muxer->WriteHeader();
	best_stream_demux_decoder->StartPump();

	// 等待退出事件到来后执行清理工作
	SDL_EventGetter event_getter;
	event_getter.WaitQuitEvent();
	best_stream_demux_decoder->Dispose();
}
