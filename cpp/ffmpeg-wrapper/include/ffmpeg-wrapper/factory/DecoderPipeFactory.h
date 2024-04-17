#pragma once
#include<ffmpeg-wrapper/info-collection/AVStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IDecoderPipe.h>

namespace video
{
	class DecoderPipeFactory
	{
	protected:
		DecoderPipeFactory() = default;

	public:
		#pragma region 单例
		static DecoderPipeFactory &Instance();

		/// <summary>
		///		派生此类，将实例赋值给本字段，可以让 Instance 方法返回自定义的工厂。
		/// </summary>
		static shared_ptr<DecoderPipeFactory> _custom_factory;
		#pragma endregion

		virtual shared_ptr<IDecoderPipe> CreateDecoderPipe(AVStreamInfoCollection const &infos);
	};
}