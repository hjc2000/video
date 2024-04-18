#include "EncoderPipeFactory.h"
#include<EncoderPipe.h>

using namespace video;

shared_ptr<EncoderPipeFactory> video::EncoderPipeFactory::_custom_factory = nullptr;

EncoderPipeFactory &video::EncoderPipeFactory::Instance()
{
	static EncoderPipeFactory instance;
	if (_custom_factory)
	{
		return *_custom_factory;
	}

	return instance;
}

shared_ptr<IFrameConsumer> video::EncoderPipeFactory::CreateEncoderPipe(
	std::string codec_name,
	IVideoStreamInfoCollection const &in_stream_infos,
	shared_ptr<OutputFormat> output_format,
	int64_t out_bit_rate_in_bps
)
{
	return shared_ptr<IFrameConsumer>{new EncoderPipe{
		codec_name,
		in_stream_infos,
		output_format,
		out_bit_rate_in_bps,
	}};
}

shared_ptr<IFrameConsumer> video::EncoderPipeFactory::CreateEncoderPipe(
	std::string codec_name,
	IAudioStreamInfoCollection const &in_stream_infos,
	shared_ptr<OutputFormat> output_format
)
{
	return shared_ptr<IFrameConsumer>{new EncoderPipe{
		codec_name,
		in_stream_infos,
		output_format
	}};
}

