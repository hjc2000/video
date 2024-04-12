#include"ffmpeg-wrapper/demux/JoinedInputFormatDemuxDecoder.h"
#include<ffmpeg-wrapper/pipe/PacketPump.h>

using namespace video;
using namespace std;

void video::JoinedInputFormatDemuxDecoder::OpenInputIfNull()
{
	if (_current_intput_format == nullptr && _get_format_callback != nullptr)
	{
		_current_intput_format = _get_format_callback();
	}

	if (_current_intput_format == nullptr)
	{
		return;
	}

	// 成功打开新的输入格式。
	if (_video_decode_pipe)
	{
		_video_decode_pipe->FlushDecoderButNotFlushConsumers();
	}

	if (_audio_decode_pipe)
	{
		_audio_decode_pipe->FlushDecoderButNotFlushConsumers();
	}

	_infinite_packet_pipe->ClearPacketConsumer();

	// 如果有视频流，初始化视频解码管道
	AVStreamWrapper stream = _current_intput_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	if (stream)
	{
		_video_stream_infos = stream;
		_source_video_stream_index = stream.Index();

		/* 视频流索引设置为 0. 向 _video_decode_pipe 送入包时也必须先将包的索引设置为 0.
		* DecoderPipe 的 SendPacket 方法会对送入的包进行过滤。如果索引号不匹配，不会解码。
		*/
		_video_stream_infos.SetIndex(0);
		// 经过 InfinitePacketPipe 处理后时间基会变成 1/90000，所以这里需要改。
		_video_stream_infos.SetTimeBase(AVRational{ 1,90000 });
		_video_decode_pipe = shared_ptr<DecoderPipe>{ new DecoderPipe{_video_stream_infos} };
		_video_decode_pipe->AddFrameConsumer(_video_frame_consumer_list);
		_infinite_packet_pipe->AddPacketConsumer(_video_decode_pipe);
	}
	else
	{
		_video_decode_pipe = nullptr;
		_source_video_stream_index = -1;
	}

	// 如果有音频流，初始化音频解码管道
	stream = _current_intput_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	if (stream)
	{
		_audio_stream_infos = stream;
		_source_audio_stream_index = stream.Index();

		/* 音频流的索引设置为 1. 向 _audio_decode_pipe 送入包时也必须先将包的索引设置为 1.
		* DecoderPipe 的 SendPacket 方法会对送入的包进行过滤。如果索引号不匹配，不会解码。
		*/
		_audio_stream_infos.SetIndex(1);
		// 经过 InfinitePacketPipe 处理后时间基会变成 1/90000，所以这里需要改。
		_audio_stream_infos.SetTimeBase(AVRational{ 1,90000 });
		_audio_decode_pipe = shared_ptr<DecoderPipe>{ new DecoderPipe{_audio_stream_infos} };
		_audio_decode_pipe->AddFrameConsumer(_audio_frame_consumer_list);
		_infinite_packet_pipe->AddPacketConsumer(_audio_decode_pipe);
	}
	else
	{
		_audio_decode_pipe = nullptr;
		_source_audio_stream_index = -1;
	}
}

void video::JoinedInputFormatDemuxDecoder::Pump(shared_ptr<CancellationToken> cancel_pump)
{
	while (!cancel_pump->IsCancellationRequested())
	{
		OpenInputIfNull();
		if (_current_intput_format == nullptr)
		{
			// 尝试打开输入后 _current_intput_format 仍然为空
			_infinite_packet_pipe->FlushConsumer();
			return;
		}

		shared_ptr<PacketPump> packet_pump{ new PacketPump{_current_intput_format} };
		packet_pump->AddPacketConsumer(_infinite_packet_pipe);
		packet_pump->_on_before_send_packet_to_consumer = [&](AVPacketWrapper *packet)
		{
			if (!packet)
			{
				return;
			}

			packet->ChangeTimeBase(AVRational{ 1,90000 });
			if (packet->StreamIndex() == _source_video_stream_index)
			{
				// 视频流的索引更改为 0.
				packet->SetStreamIndex(0);
			}
			else if (packet->StreamIndex() == _source_audio_stream_index)
			{
				// 音频流的索引更改为 1.
				packet->SetStreamIndex(1);
			}
		};
		packet_pump->Pump(cancel_pump);
		_current_intput_format = nullptr;
	}
}

void video::JoinedInputFormatDemuxDecoder::AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer)
{
	_video_frame_consumer_list.Add(consumer);
}

void video::JoinedInputFormatDemuxDecoder::AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer)
{
	_audio_frame_consumer_list.Add(consumer);
}

AVStreamInfoCollection const &video::JoinedInputFormatDemuxDecoder::GetVideoStreamInfos()
{
	OpenInputIfNull();
	if (_current_intput_format == nullptr)
	{
		// 尝试打开输入后 _current_intput_format 仍然为空
		throw jc::Exception("当前没有视频流信息可读");
	}

	return _video_stream_infos;
}

AVStreamInfoCollection const &video::JoinedInputFormatDemuxDecoder::GetAudioStreamInfos()
{
	OpenInputIfNull();
	if (_current_intput_format == nullptr)
	{
		// 尝试打开输入后 _current_intput_format 仍然为空
		throw jc::Exception("当前没有音频流信息可读");
	}

	return _audio_stream_infos;
}
