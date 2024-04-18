#include"BestStreamDemuxDecoder.h"

using namespace std;
using namespace video;

void video::BestStreamDemuxDecoder::InitializeVideoDecoderPipe()
{
}

void video::BestStreamDemuxDecoder::InitializeAudioDecoderPipe()
{
}

video::BestStreamDemuxDecoder::BestStreamDemuxDecoder(shared_ptr<InputFormat> input_format)
{
}

void video::BestStreamDemuxDecoder::AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer)
{
}

void video::BestStreamDemuxDecoder::AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer)
{
}

void video::BestStreamDemuxDecoder::Pump(shared_ptr<CancellationToken> cancel_pump)
{
}
