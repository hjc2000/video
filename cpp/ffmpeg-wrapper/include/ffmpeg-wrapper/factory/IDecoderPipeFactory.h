#pragma once
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>

namespace video
{
	class IDecoderPipeFactory
	{
	public:
		virtual ~IDecoderPipeFactory() = default;

		virtual shared_ptr<IDecoderPipe> CreateDecoderPipe(AVStreamInfoCollection const &infos) = 0;
	};
}