#include "EncoderPipeFactory.h"
#include<ffmpeg-wrapper/pipe/EncoderPipe.h>

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
	IVideoStreamInfoCollection &in_stream_infos,
	shared_ptr<OutputFormat> output_format,
	int64_t out_bit_rate_in_bps
)
{
	return shared_ptr<IFrameConsumer>();
}

