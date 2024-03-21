#include"SptsEncodeMux.h"

using namespace video;
using namespace std;

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

shared_ptr<IFrameConsumer> video::SptsEncodeMux::GetVideoEncodePipe()
{
	return _video_encode_pipe;
}

shared_ptr<IFrameConsumer> video::SptsEncodeMux::GetAudioEncodePipe()
{
	return _audio_encode_pipe;
}
