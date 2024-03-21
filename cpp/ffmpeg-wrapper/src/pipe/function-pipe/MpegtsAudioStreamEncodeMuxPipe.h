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
			IAudioStreamInfoCollection &desired_out_stream_infos,
			std::string codec_name
		)
		{
			_out_fmt_ctx = out_fmt_ctx;
			_desired_out_stream_infos = desired_out_stream_infos;
			_codec_name = codec_name;

			_desired_out_stream_infos._time_base = AVRational{ 1,90000 };
			_encode_pipe = shared_ptr<SwrEncoderPipe>{
				new SwrEncoderPipe{
					_codec_name.c_str(),
					_desired_out_stream_infos,
					_out_fmt_ctx,
				}
			};
		}

	private:
		shared_ptr<OutputFormat> _out_fmt_ctx;
		AudioStreamInfoCollection _desired_out_stream_infos;
		std::string _codec_name;

		shared_ptr<SwrEncoderPipe> _encode_pipe;

	public:
		void SendFrame(AVFrameWrapper *frame) override
		{
			_encode_pipe->SendFrame(frame);
		}
	};
}