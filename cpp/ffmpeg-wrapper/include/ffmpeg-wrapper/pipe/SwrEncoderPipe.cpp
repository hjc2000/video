#include "ffmpeg-wrapper/pipe/SwrEncoderPipe.h"
#include<ffmpeg-wrapper/AVSampleFormatExtention.h>
#include<ffmpeg-wrapper/factory/EncoderPipeFactory.h>

void video::SwrEncoderPipe::SendFrame(AVFrameWrapper *frame)
{
	_swr_pipe->SendFrame(frame);
}

video::SwrEncoderPipe::SwrEncoderPipe(
	std::string codec_name,
	IAudioStreamInfoCollection &desire_encode_out_stream_infos,
	shared_ptr<OutputFormat> output_format
)
{
	AudioFrameInfoCollection swr_out_frame_infos{
		desire_encode_out_stream_infos,
		AVSampleFormatExtention::ParseRequiredSampleCount(codec_name)
	};
	_swr_pipe = shared_ptr<SwrPipe>{ new SwrPipe{swr_out_frame_infos} };
	_encoder_pipe = EncoderPipeFactory::Instance().CreateEncoderPipe(
		codec_name,
		desire_encode_out_stream_infos,
		output_format
	);
	_swr_pipe->FrameConsumerList().Add(_encoder_pipe);
}
