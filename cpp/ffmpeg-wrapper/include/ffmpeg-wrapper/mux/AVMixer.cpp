#include"AVMixer.h"

using namespace std;
using namespace video;

void video::AVMixer::CreateNewVideoStream()
{
	_input_video_format->DumpFormat();
	AVStreamWrapper stream = _input_video_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	_src_video_stream_index = stream.Index();
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(0);
}

void video::AVMixer::CreateNewAudioStream()
{
	_input_audio_format->DumpFormat();
	AVStreamWrapper stream = _input_audio_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	_src_audio_stream_index = stream.Index();
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(1);
}

bool video::AVMixer::ReadVideoPacketOnce(shared_ptr<CancellationToken> cancel_pump)
{
	if (_video_stream_end)
	{
		return false;
	}

	while (!cancel_pump->IsCancellationRequested())
	{
		int result = _input_video_format->ReadPacket(_temp_packet);
		switch (result)
		{
		case 0:
			{
				if (_temp_packet.StreamIndex() != _src_video_stream_index)
				{
					continue;
				}

				_temp_packet.SetStreamIndex(0);
				_video_time = _temp_packet.pts() * 1000 * _temp_packet.TimeBase();
				_out_format->SendPacket(&_temp_packet);
				if (_video_time < _audio_time)
				{
					continue;
				}

				return true;
			}
		default:
			{
				_video_stream_end = true;
				return false;
			}
		}
	}

	return false;
}

bool video::AVMixer::ReadAudioPacketOnce(shared_ptr<CancellationToken> cancel_pump)
{
	if (_audio_stream_end)
	{
		return false;
	}

	while (!cancel_pump->IsCancellationRequested())
	{
		int result = _input_audio_format->ReadPacket(_temp_packet);
		switch (result)
		{
		case 0:
			{
				if (_temp_packet.StreamIndex() != _src_audio_stream_index)
				{
					continue;
				}

				_temp_packet.SetStreamIndex(1);
				_audio_time = _temp_packet.pts() * 1000 * _temp_packet.TimeBase();
				_out_format->SendPacket(&_temp_packet);
				if (_audio_time < _video_time)
				{
					continue;
				}

				return true;
			}
		default:
			{
				_audio_stream_end = true;
				return false;
			}
		}
	}

	return false;
}

video::AVMixer::AVMixer(
	shared_ptr<InputFormat> input_video_format,
	shared_ptr<InputFormat> input_audio_format,
	shared_ptr<OutputFormat> out_format
)
{
	_input_video_format = input_video_format;
	_input_audio_format = input_audio_format;
	_out_format = out_format;

	CreateNewVideoStream();
	CreateNewAudioStream();
}

void video::AVMixer::Pump(shared_ptr<CancellationToken> cancel_pump)
{
	_out_format->WriteHeader();
	_out_format->DumpFormat();
	while (!cancel_pump->IsCancellationRequested())
	{
		bool read_video_packet_result = ReadVideoPacketOnce(cancel_pump);
		bool read_audio_packet_result = ReadAudioPacketOnce(cancel_pump);
		if (!read_video_packet_result && !read_audio_packet_result)
		{
			_out_format->SendPacket(nullptr);
			_out_format->SendPacket(nullptr);
			return;
		}
	}
}

#include<ffmpeg-wrapper/output-format/FileOutputFormat.h>

void video::TestAVMixer()
{
	shared_ptr<InputFormat> input_video_format{ new InputFormat{"渡尘.mp4"} };
	shared_ptr<InputFormat> input_audio_format{ new InputFormat{"idol.mp4"} };
	shared_ptr<FileOutputFormat> output_format{ new FileOutputFormat{"out.mp4"} };
	AVMixer mix{ input_video_format,input_audio_format,output_format };
	CancellationTokenSource cancel_pump_source;
	mix.Pump(cancel_pump_source.Token());
}
