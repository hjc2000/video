#include "ffmpeg-wrapper/pipe/SwrEncoderPipe.h"

void video::SwrEncoderPipe::SendFrame(AVFrameWrapper *frame)
{
	_swr_pipe->SendFrame(frame);
}

video::SwrEncoderPipe::SwrEncoderPipe(
	char const *codec_name,
	IAudioStreamInfoCollection &desire_encode_out_stream_infos,
	shared_ptr<OutputFormat> output_format
)
{
	AudioFrameInfoCollection swr_out_frame_infos{
		desire_encode_out_stream_infos,
		ParseRequiredSampleCount(codec_name)
	};
	_swr_pipe = shared_ptr<SwrPipe>{ new SwrPipe{swr_out_frame_infos} };

	_encoder_pipe = shared_ptr<EncoderPipe>{
		new EncoderPipe{
			codec_name,
			desire_encode_out_stream_infos,
			output_format
	}
	};

	_swr_pipe->AddFrameConsumer(_encoder_pipe);
}

int video::SwrEncoderPipe::ParseRequiredSampleCount(std::string codec_name)
{
	int nb_samples = 1024;

	// 根据编码器来确定采样点数量。编码器对采样点数量有要求。
	if (std::string(codec_name) == "aac")
	{
		nb_samples = 1024;
	}

	return nb_samples;
}
