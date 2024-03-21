#include"InfiniteBestStreamDemuxDecoder.h"

using namespace video;

video::InfiniteBestStreamDemuxDecoder_old::InfiniteBestStreamDemuxDecoder_old(
	std::function<shared_ptr<InputFormatContext>()> get_format_callback
)
{
	_get_format_callback = get_format_callback;
	if (!OpenInput())
	{
		cout << CODE_POS_STR << "第一次尝试打开封装就失败了" << endl;
		throw jc::Exception();
	}
}

bool video::InfiniteBestStreamDemuxDecoder_old::OpenInput()
{
	try
	{
		_in_fmt_ctx = _get_format_callback();
	}
	catch (...)
	{
		cout << CODE_POS_STR << "_get_format_callback 发生异常" << endl;
		throw;
	}

	if (!_in_fmt_ctx)
	{
		return false;
	}

	_src_video_stream_index = -1;
	_src_audio_stream_index = -1;
	_stream_list.Clear();
	AVStreamWrapper stream;
	stream = _in_fmt_ctx->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (stream)
	{
		_src_video_stream_index = stream.Index();

		AVStreamInfoCollection best_video_stream_infos = stream;
		best_video_stream_infos._timebase = AVRational{ 1,90000 };
		// 视频流的索引更改为 0.
		best_video_stream_infos._stream_index = 0;

		_stream_list.Add(best_video_stream_infos);
	}

	stream = _in_fmt_ctx->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (stream)
	{
		_src_audio_stream_index = stream.Index();

		AVStreamInfoCollection best_audio_stream_infos = stream;
		best_audio_stream_infos._timebase = AVRational{ 1,90000 };
		// 音频流的索引更改为 1.
		best_audio_stream_infos._stream_index = 1;

		_stream_list.Add(best_audio_stream_infos);
	}

	if (!_program_decoder)
	{
		_program_decoder = shared_ptr<ProgramDecoder>{ new ProgramDecoder{_stream_list} };
		_infinite_packet_pipe->AddPacketConsumer(_program_decoder);
	}
	else
	{
		_program_decoder->ReplaceDecodePipe(_stream_list);
	}

	return true;
}

void video::InfiniteBestStreamDemuxDecoder_old::StartPump()
{
	if (_pump_started) return;
	_pump_started = true;

	_pump_thread_has_exit.Reset();
	std::thread([&]()
	{
		try
		{
			while (!_cancel_pump.IsCancellationRequested())
			{
				shared_ptr<PacketPump> packet_pump{ new PacketPump{_in_fmt_ctx} };
				packet_pump->_on_before_send_packet_to_consumer = [&](AVPacketWrapper *packet)
				{
					if (!packet)
					{
						return;
					}

					packet->ChangeTimeBase(AVRational{ 1,90000 });
					if (packet->stream_index() == _src_video_stream_index)
					{
						// 视频流的索引更改为 0.
						packet->set_stream_index(0);
					}
					else if (packet->stream_index() == _src_audio_stream_index)
					{
						// 音频流的索引更改为 1.
						packet->set_stream_index(1);
					}
				};

				packet_pump->AddPacketConsumer(_infinite_packet_pipe);
				packet_pump->Pump(_cancel_pump.Token());
				if (_cancel_pump.IsCancellationRequested())
				{
					break;
				}

				if (!OpenInput())
				{
					break;
				}
			}
		}
		catch (jc::Exception &e)
		{
			cout << e.what() << endl;
		}
		catch (...)
		{

		}

		// 线程退出
		_infinite_packet_pipe->FlushConsumer();
		_pump_thread_has_exit.SetResult();
		cout << CODE_POS_STR << "Pump 线程退出" << endl;
	}).detach();
}

List<AVStreamInfoCollection> &video::InfiniteBestStreamDemuxDecoder_old::StreamList()
{
	return _program_decoder->StreamList();
}

shared_ptr<IPipeFrameSource> video::InfiniteBestStreamDemuxDecoder_old::GetPipeFrameSourceByStreamIndex(int stream_index)
{
	return _program_decoder->GetPipeFrameSourceByStreamIndex(stream_index);
}
