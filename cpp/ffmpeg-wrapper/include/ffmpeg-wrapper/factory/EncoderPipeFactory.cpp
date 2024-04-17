#include "EncoderPipeFactory.h"

using namespace video;

EncoderPipeFactory &video::EncoderPipeFactory::Instance()
{
	static EncoderPipeFactory instance;
	if (_custom_factory)
	{
		return *_custom_factory;
	}

	return instance;
}
