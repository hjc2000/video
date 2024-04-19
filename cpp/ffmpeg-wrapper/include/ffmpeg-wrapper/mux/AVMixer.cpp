#include"AVMixer.h"

using namespace std;
using namespace video;

void video::AVMixer::CreateNewVideoStream()
{
	AVStreamWrapper video_stream = _input_video_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
	AVStreamWrapper new_video_stream = _out_format->CreateNewStream();
	new_video_stream.SetCodecParams(video_stream.CodecParams());
	new_video_stream.SetIndex(0);
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
}

void video::AVMixer::Pump(shared_ptr<CancellationToken> cancel_pump)
{

}
