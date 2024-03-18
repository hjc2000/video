#include"MpegtsVideoStreamEncodeMuxPipe.h"

using namespace video;

void MpegtsVideoStreamEncodeMuxPipe::InitEncodePipe()
{
	try
	{
		_encode_pipe = shared_ptr<SwsFpsEncoderPipe>{
			new SwsFpsEncoderPipe{
				_codec_name.c_str(),
				_out_stream_infos,
				_out_fmt_ctx,
				_out_bitrate,
				_display_frame
		}
		};
	}
	catch (...)
	{
		_encode_pipe = shared_ptr<SwsFpsEncoderPipe>{
			new SwsFpsEncoderPipe{
				"libx265",
				_out_stream_infos,
				_out_fmt_ctx,
				_out_bitrate,
				_display_frame
		}
		};
	}

	if (_pid > 0)
	{
		_encode_pipe->NewVideoStream()->id = _pid;
	}
}
