#pragma once
#include<ffmpeg-wrapper/pipe/ThreadDecoderPipe.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<ffmpeg-wrapper/pipe/interface/IPump.h>

namespace video
{
	class BestStreamDemuxDecoder :public IPump
	{
	public:
		void AddVideoFrameConsumer(shared_ptr<IFrameConsumer> consumer);
		void AddAudioFrameConsumer(shared_ptr<IFrameConsumer> consumer);

		// 通过 IPump 继承
		void Pump(shared_ptr<CancellationToken> cancel_pump) override;
	};
}