#pragma once
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPump.h>

namespace video
{
	class BestStreamDemuxDecoder :public IPump
	{
		shared_ptr<InputFormat> _input_format;
		shared_ptr<IDecoderPipe> _video_decode_pipe;
		shared_ptr<IDecoderPipe> _audio_decode_pipe;

	public:
		BestStreamDemuxDecoder(shared_ptr<InputFormat> input_format);

		void AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer);
		void AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer);

		void Pump(shared_ptr<CancellationToken> cancel_pump) override;
	};
}