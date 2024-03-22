#include"JoinedInputFormatDemuxDecoder.h"
#include<PacketPump.h>

using namespace video;
using namespace std;

void video::JoinedInputFormatDemuxDecoder::OpenInputIfNull()
{
	if (_current_intput_format == nullptr && _get_format_callback != nullptr)
	{
		_current_intput_format = _get_format_callback();
	}

	// 如果有视频流，初始化视频解码管道
	AVStreamWrapper stream = _current_intput_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (stream)
	{
		_video_stream_infos = stream;

		/* 视频流索引设置为 0. 向 _video_decode_pipe 送入包时也必须先将包的索引设置为 0. */
		_video_stream_infos.SetIndex(0);
		_video_decode_pipe = shared_ptr<DecoderPipe>{ new DecoderPipe{_video_stream_infos} };
	}
	else
	{
		_video_decode_pipe = nullptr;
	}

	// 如果有音频流，初始化音频解码管道
	stream = _current_intput_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (stream)
	{
		_audio_stream_infos = stream;

		/* 音频流的索引设置为 1. 向 _audio_decode_pipe 送入包时也必须先将包的索引设置为 1. */
		_audio_stream_infos.SetIndex(1);
		_audio_decode_pipe = shared_ptr<DecoderPipe>{ new DecoderPipe{_audio_stream_infos} };
	}
	else
	{
		_audio_decode_pipe = nullptr;
	}
}

void video::JoinedInputFormatDemuxDecoder::Pump(shared_ptr<CancellationToken> cancel_pump)
{
	while (!cancel_pump->IsCancellationRequested())
	{
		shared_ptr<PacketPump> packet_pump{ new PacketPump{_current_intput_format} };
		packet_pump->_on_before_send_packet_to_consumer = [&](AVPacketWrapper *packet)
		{
			if (!packet)
			{
				return;
			}
		};
	}
}
