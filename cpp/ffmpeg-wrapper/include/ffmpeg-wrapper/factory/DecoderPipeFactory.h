#pragma once
#include<ffmpeg-wrapper/factory/IDecoderPipeFactory.h>
#include<memory>

namespace video
{
	class DecoderPipeFactory :
		public video::IDecoderPipeFactory
	{
	protected:
		DecoderPipeFactory() = default;

	public:
		static std::shared_ptr<DecoderPipeFactory> Instance();

		shared_ptr<IDecoderPipe> CreateDecoderPipe(AVStreamInfoCollection const &infos) override;
	};
}