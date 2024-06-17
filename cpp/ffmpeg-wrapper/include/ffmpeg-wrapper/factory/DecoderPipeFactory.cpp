#include"DecoderPipeFactory.h"
#include<DecoderPipe.h>

using namespace std;
using namespace video;

DecoderPipeFactory &DecoderPipeFactory::Instance()
{
	static DecoderPipeFactory o;
	return o;
}

shared_ptr<IDecoderPipe> DecoderPipeFactory::CreateDecoderPipe(AVStreamInfoCollection const &infos)
{
	return shared_ptr<IDecoderPipe>{new DecoderPipe { infos }};
}
