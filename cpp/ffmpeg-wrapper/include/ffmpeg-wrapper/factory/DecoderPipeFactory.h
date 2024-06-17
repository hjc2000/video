#pragma once
#include<ffmpeg-wrapper/factory/IDecoderPipeFactory.h>

namespace video
{
	class DecoderPipeFactory :public video::IDecoderPipeFactory
	{
	protected:
		DecoderPipeFactory() = default;

	public:
		static DecoderPipeFactory &Instance();

		shared_ptr<IDecoderPipe> CreateDecoderPipe(AVStreamInfoCollection const &infos) override;
	};
}