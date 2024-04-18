#pragma once
#include<ffmpeg-wrapper/input-format/InputFormat.h>
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPump.h>

namespace video
{
	class BestStreamDemuxDecoder :public IPump
	{
		shared_ptr<InputFormat> _intput_format;
		AVStreamInfoCollection _video_stream_infos;
		shared_ptr<IDecoderPipe> _video_decode_pipe;
		int _source_video_stream_index = -1;
		AVStreamInfoCollection _audio_stream_infos;
		shared_ptr<IDecoderPipe> _audio_decode_pipe;
		int _source_audio_stream_index = -1;
		List<shared_ptr<IFrameConsumer>> _video_frame_consumer_list;
		List<shared_ptr<IFrameConsumer>> _audio_frame_consumer_list;

		void InitializeVideoDecoderPipe();
		void InitializeAudioDecoderPipe();

	public:
		BestStreamDemuxDecoder(shared_ptr<InputFormat> input_format);

		void AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer);
		void AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer);
		void Pump(shared_ptr<CancellationToken> cancel_pump) override;
	};
}