#include"DecoderPipeFactory.h"
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>

using namespace std;
using namespace video;

DecoderPipeFactory &DecoderPipeFactory::Instance()
{
	static DecoderPipeFactory instance;
	if (_custom_factory)
	{
		return *_custom_factory;
	}

	return instance;
}

shared_ptr<DecoderPipeFactory> DecoderPipeFactory::_custom_factory = nullptr;

shared_ptr<IDecoderPipe> DecoderPipeFactory::CreateDecoderPipe(AVStreamInfoCollection const &infos)
{
	return shared_ptr<IDecoderPipe>{new ThreadDecoderPipe{ infos }};
}
