#pragma once
#include<AudioStreamInfoCollection.h>
#include<SwrEncoderPipe.h>

namespace video
{
	class MpegtsAudioStreamEncodeMuxPipe :public IFrameConsumer
	{
	public:
		MpegtsAudioStreamEncodeMuxPipe(
			shared_ptr<OutputFormat> out_fmt_ctx,
			IAudioStreamInfoCollection &out_stream_infos,
			std::string codec_name,
			int pid
		)
		{
			_out_fmt_ctx = out_fmt_ctx;
			_stream_infos = out_stream_infos;
			_codec_name = codec_name;
			_pid = pid;

			_stream_infos._time_base = AVRational{ 1,90000 };
			_encode_pipe = shared_ptr<SwrEncoderPipe>{
				new SwrEncoderPipe{
					_codec_name.c_str(),
					_stream_infos,
					_out_fmt_ctx,
				}
			};

			if (_pid > 0)
			{
				_encode_pipe->NewAudioStream()->id = _pid;
			}
		}

	private:
		shared_ptr<OutputFormat> _out_fmt_ctx;
		AudioStreamInfoCollection _stream_infos;
		std::string _codec_name;
		int _pid;

		shared_ptr<SwrEncoderPipe> _encode_pipe;

	public:
		void SendFrame(AVFrameWrapper *frame) override
		{
			_encode_pipe->SendFrame(frame);
		}
	};
}