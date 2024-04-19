#include"AVMixer.h"

using namespace std;
using namespace video;

void video::AVMixer::CreateNewVideoStream()
{
	AVStreamWrapper stream = _input_video_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(0);
}

void video::AVMixer::CreateNewAudioStream()
{
	AVStreamWrapper stream = _input_video_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
	AVStreamWrapper new_stream = _out_format->CreateNewStream();
	new_stream.SetCodecParams(stream.CodecParams());
	new_stream.SetIndex(1);
}

bool video::AVMixer::ReadVideoPacketOnce()
{
	int result = _input_video_format->ReadPacket(_temp_packet);
	switch (result)
	{
	case 0:
		{
			_temp_packet.SetStreamIndex(0);
			_infinite_packet_pipe->SendPacket(&_temp_packet);
			return true;
		}
	default:
		{
			return false;
		}
	}
}

bool video::AVMixer::ReadAudioPacketOnce()
{
	int result = _input_video_format->ReadPacket(_temp_packet);
	switch (result)
	{
	case 0:
		{
			_temp_packet.SetStreamIndex(1);
			_infinite_packet_pipe->SendPacket(&_temp_packet);
			return true;
		}
	default:
		{
			return false;
		}
	}
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
	_infinite_packet_pipe->PacketConsumerList().Add(_out_format);

	CreateNewVideoStream();
	CreateNewAudioStream();
}

void video::AVMixer::Pump(shared_ptr<CancellationToken> cancel_pump)
{
	while (!cancel_pump->IsCancellationRequested())
	{
		if (!ReadVideoPacketOnce() && !ReadAudioPacketOnce())
		{
			_infinite_packet_pipe->FlushConsumer();
			return;
		}
	}
}
