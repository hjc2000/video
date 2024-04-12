#include"ThreadDecoderPipe.h"

using namespace video;

video::ThreadDecoderPipe::ThreadDecoderPipe(AVStreamInfoCollection stream)
{
	_decoder_pipe = shared_ptr<DecoderPipe>{ new DecoderPipe{stream} };
}

void video::ThreadDecoderPipe::Dispose()
{
	_decoder_pipe->Dispose();
}

void video::ThreadDecoderPipe::SendPacket(AVPacketWrapper *packet)
{
}

shared_ptr<AVCodecContextWrapper> video::ThreadDecoderPipe::DecoderContext()
{
	return _decoder_pipe->DecoderContext();
}

void video::ThreadDecoderPipe::FlushDecoderButNotFlushConsumers()
{
}

AVRational video::ThreadDecoderPipe::TimeBase() const
{
	return _decoder_pipe->TimeBase();
}

void video::ThreadDecoderPipe::SetTimeBase(AVRational value)
{
	_decoder_pipe->SetTimeBase(value);
}

AVSampleFormat video::ThreadDecoderPipe::SampleFormat() const
{
	return _decoder_pipe->SampleFormat();
}

void video::ThreadDecoderPipe::SetSampleFormat(AVSampleFormat value)
{
	_decoder_pipe->SetSampleFormat(value);
}

int video::ThreadDecoderPipe::SampleRate() const
{
	return _decoder_pipe->SampleRate();
}

void video::ThreadDecoderPipe::SetSampleRate(int value)
{
	_decoder_pipe->SetSampleRate(value);
}

AVChannelLayout video::ThreadDecoderPipe::ChannelLayout() const
{
	return _decoder_pipe->ChannelLayout();
}

void video::ThreadDecoderPipe::SetChannelLayout(AVChannelLayout value)
{
	_decoder_pipe->SetChannelLayout(value);
}

int video::ThreadDecoderPipe::Width() const
{
	return _decoder_pipe->Width();
}

void video::ThreadDecoderPipe::SetWidth(int value)
{
	_decoder_pipe->SetWidth(value);
}

int video::ThreadDecoderPipe::Height() const
{
	return _decoder_pipe->Height();
}

void video::ThreadDecoderPipe::SetHeight(int value)
{
	_decoder_pipe->SetHeight(value);
}

AVPixelFormat video::ThreadDecoderPipe::PixelFormat() const
{
	return _decoder_pipe->PixelFormat();
}

void video::ThreadDecoderPipe::SetPixelFormat(AVPixelFormat value)
{
	_decoder_pipe->SetPixelFormat(value);
}

AVRational video::ThreadDecoderPipe::FrameRate() const
{
	return _decoder_pipe->FrameRate();
}

void video::ThreadDecoderPipe::SetFrameRate(AVRational value)
{
	_decoder_pipe->SetFrameRate(value);
}
